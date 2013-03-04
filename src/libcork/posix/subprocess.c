/* -*- coding: utf-8 -*-
 * ----------------------------------------------------------------------
 * Copyright © 2012, RedJack, LLC.
 * All rights reserved.
 *
 * Please see the COPYING file in this distribution for license
 * details.
 * ----------------------------------------------------------------------
 */

#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/select.h>
#include <sys/wait.h>
#include <unistd.h>

#include "libcork/core.h"
#include "libcork/ds.h"
#include "libcork/os/subprocess.h"
#include "libcork/threads/basics.h"
#include "libcork/helpers/errors.h"


#if !defined(CORK_DEBUG_SUBPROCESS)
#define CORK_DEBUG_SUBPROCESS  0
#endif

#if CORK_DEBUG_SUBPROCESS
#include <stdio.h>
#define DEBUG(...) fprintf(stderr, __VA_ARGS__)
#else
#define DEBUG(...) /* no debug messages */
#endif


#define ri_check_posix(call) \
    do { \
        while (true) { \
            if ((call) == -1) { \
                if (errno == EINTR) { \
                    continue; \
                } else { \
                    cork_system_error_set(); \
                    CORK_PRINT_ERROR(); \
                    return -1; \
                } \
            } else { \
                break; \
            } \
        } \
    } while (0)

#define e_check_posix(call) \
    do { \
        while (true) { \
            if ((call) == -1) { \
                if (errno == EINTR) { \
                    continue; \
                } else { \
                    cork_system_error_set(); \
                    CORK_PRINT_ERROR(); \
                    goto error; \
                } \
            } else { \
                break; \
            } \
        } \
    } while (0)


/*-----------------------------------------------------------------------
 * Subprocess groups
 */

#define BUF_SIZE  4096

struct cork_subprocess_group {
    cork_array(struct cork_subprocess *)  subprocesses;
    size_t  still_running;
    struct sigaction  old_chld;
    struct sigaction  old_hup;
    struct sigaction  old_int;
    struct sigaction  old_term;
    char  read_buf[BUF_SIZE];
};

struct cork_subprocess_group *
cork_subprocess_group_new(void)
{
    struct cork_subprocess_group  *group =
        cork_new(struct cork_subprocess_group);
    cork_array_init(&group->subprocesses);
    return group;
}

void
cork_subprocess_group_free(struct cork_subprocess_group *group)
{
    size_t  i;
    assert(group->still_running == 0);
    for (i = 0; i < cork_array_size(&group->subprocesses); i++) {
        cork_subprocess_free(cork_array_at(&group->subprocesses, i));
    }
    cork_array_done(&group->subprocesses);
    free(group);
}

void
cork_subprocess_group_add(struct cork_subprocess_group *group,
                          struct cork_subprocess *sub)
{
    cork_array_append(&group->subprocesses, sub);
}


/*-----------------------------------------------------------------------
 * Pipes
 */

struct cork_pipe {
    struct cork_stream_consumer  *consumer;
    int  fds[2];
    bool  first;
};

static void
cork_pipe_init(struct cork_pipe *p, struct cork_stream_consumer *consumer)
{
    p->consumer = consumer;
    p->fds[0] = -1;
    p->fds[1] = -1;
}

static int
cork_pipe_close_read(struct cork_pipe *p)
{
    if (p->fds[0] != -1) {
        DEBUG("Closing read pipe %d\n", p->fds[0]);
        ri_check_posix(close(p->fds[0]));
        p->fds[0] = -1;
    }
    return 0;
}

static int
cork_pipe_close_write(struct cork_pipe *p)
{
    if (p->fds[1] != -1) {
        DEBUG("Closing write pipe %d\n", p->fds[1]);
        ri_check_posix(close(p->fds[1]));
        p->fds[1] = -1;
    }
    return 0;
}

static void
cork_pipe_close(struct cork_pipe *p)
{
    cork_pipe_close_read(p);
    cork_pipe_close_write(p);
}

static void
cork_pipe_done(struct cork_pipe *p)
{
    cork_pipe_close(p);
}

static int
cork_pipe_open(struct cork_pipe *p)
{
    if (p->consumer != NULL) {
        int  flags;

        /* We want the read end of the pipe to be non-blocking. */
        DEBUG("Opening pipe\n");
        ri_check_posix(pipe(p->fds));
        DEBUG("  Got read=%d write=%d\n", p->fds[0], p->fds[1]);
        DEBUG("  Setting non-blocking flag on read pipe\n");
        e_check_posix(flags = fcntl(p->fds[0], F_GETFD));
        flags |= O_NONBLOCK;
        e_check_posix(fcntl(p->fds[0], F_SETFD, flags));
    }

    p->first = true;
    return 0;

error:
    cork_pipe_close(p);
    return -1;
}

static int
cork_pipe_dup(struct cork_pipe *p, int fd)
{
    if (p->fds[1] != -1) {
        ri_check_posix(dup2(p->fds[1], fd));
    }
    return 0;
}

static void
cork_pipe_set_fd(struct cork_pipe *p, fd_set *fds, int *nfds)
{
    if (p->fds[0] != -1) {
        DEBUG("  Adding read pipe %d\n", p->fds[0]);
        FD_SET(p->fds[0], fds);
        if (p->fds[0] >= *nfds) {
            *nfds = p->fds[0] + 1;
        }
    }
}

static int
cork_pipe_read(struct cork_pipe *p, struct cork_subprocess_group *group,
               fd_set *fds)
{
    if (p->fds[0] == -1 || !FD_ISSET(p->fds[0], fds)) {
        return 0;
    }

    do {
        DEBUG("Reading from pipe %d\n", p->fds[0]);
        ssize_t  bytes_read = read(p->fds[0], group->read_buf, BUF_SIZE);
        if (bytes_read == -1) {
            if (errno == EAGAIN) {
                /* We've exhausted all of the data currently available. */
                DEBUG("  No more bytes without blocking\n");
                return 0;
            } else if (errno == EINTR) {
                /* Interrupted by a signal; return so that our wait loop can
                 * catch that. */
                DEBUG("  Interrupted by signal\n");
                return 0;
            } else {
                /* An actual error */
                cork_system_error_set();
                DEBUG("  Error: %s\n", cork_error_message());
                return -1;
            }
        } else if (bytes_read == 0) {
            DEBUG("  End of stream\n");
            rii_check(cork_stream_consumer_eof(p->consumer));
            ri_check_posix(close(p->fds[0]));
            p->fds[0] = -1;
            return 0;
        } else {
            DEBUG("  Got %zd bytes\n", bytes_read);
            rii_check(cork_stream_consumer_data
                      (p->consumer, group->read_buf, bytes_read, p->first));
            p->first = false;
        }
    } while (true);
}


/*-----------------------------------------------------------------------
 * Subprocesses
 */

struct cork_subprocess {
    pid_t  pid;
    struct cork_pipe  stdout_pipe;
    struct cork_pipe  stderr_pipe;
    struct cork_thread_body  *body;
    struct cork_env  *env;
    int  *exit_code;
};

struct cork_subprocess *
cork_subprocess_new(struct cork_thread_body *body, struct cork_env *env,
                    struct cork_stream_consumer *stdout_consumer,
                    struct cork_stream_consumer *stderr_consumer,
                    int *exit_code)
{
    struct cork_subprocess  *self = cork_new(struct cork_subprocess);
    cork_pipe_init(&self->stdout_pipe, stdout_consumer);
    cork_pipe_init(&self->stderr_pipe, stderr_consumer);
    self->pid = 0;
    self->body = body;
    self->env = env;
    self->exit_code = exit_code;
    return self;
}

void
cork_subprocess_free(struct cork_subprocess *self)
{
    cork_thread_body_free(self->body);
    if (self->env != NULL) {
        cork_env_free(self->env);
    }
    cork_pipe_done(&self->stdout_pipe);
    cork_pipe_done(&self->stderr_pipe);
    free(self);
}


/*-----------------------------------------------------------------------
 * Executing another program
 */

struct cork_exec {
    struct cork_thread_body  parent;
    const char  *program;
    char * const  *params;
};

static int
cork_exec__run(struct cork_thread_body *vself)
{
    struct cork_exec  *self = cork_container_of(vself, struct cork_exec, parent);
    /* Execute the program */
    e_check_posix(execvp(self->program, self->params));

error:
    /* If we fall through, there was an error execing the subprocess. */
    _exit(EXIT_FAILURE);
}

static void
cork_exec__free(struct cork_thread_body *vself)
{
    struct cork_exec  *self = cork_container_of(vself, struct cork_exec, parent);
    free(self);
}

static struct cork_thread_body *
cork_exec_new(const char *program, char * const *params)
{
    struct cork_exec  *self = cork_new(struct cork_exec);
    self->parent.run = cork_exec__run;
    self->parent.free = cork_exec__free;
    self->program = program;
    self->params = params;
    return &self->parent;
}

struct cork_subprocess *
cork_subprocess_new_exec(const char *program, char * const *params,
                         struct cork_env *env,
                         struct cork_stream_consumer *out,
                         struct cork_stream_consumer *err,
                         int *exit_code)
{
    struct cork_thread_body  *body = cork_exec_new(program, params);
    return cork_subprocess_new(body, env, out, err, exit_code);
}


/*-----------------------------------------------------------------------
 * Running subprocesses
 */

static int
cork_subprocess_fork(struct cork_subprocess *self)
{
    pid_t  pid;

    /* Create the stdout and stderr pipes. */
    if (cork_pipe_open(&self->stdout_pipe) == -1) {
        return -1;
    }
    if (cork_pipe_open(&self->stderr_pipe) == -1) {
        cork_pipe_close(&self->stdout_pipe);
        return -1;
    }

    /* Fork the child process. */
    DEBUG("Forking child process\n");
    pid = fork();
    if (pid == 0) {
        /* Child process */

        /* Close the parent's end of the pipes */
        DEBUG("[child] ");
        cork_pipe_close_read(&self->stdout_pipe);
        DEBUG("[child] ");
        cork_pipe_close_read(&self->stderr_pipe);

        /* Bind the stdout and stderr pipes */
        if (cork_pipe_dup(&self->stdout_pipe, STDOUT_FILENO) == -1) {
            _exit(EXIT_FAILURE);
        }
        if (cork_pipe_dup(&self->stderr_pipe, STDERR_FILENO) == -1) {
            _exit(EXIT_FAILURE);
        }

        /* Fill in the requested environment */
        if (self->env != NULL) {
            cork_env_replace_current(self->env);
        }

        /* Run the subprocess's body */
        cork_thread_body_run(self->body);
        _exit(EXIT_SUCCESS);
    } else if (pid < 0) {
        /* Error forking */
        cork_system_error_set();
        return -1;
    } else {
        /* Parent process */
        DEBUG("  Child PID=%d\n", (int) pid);
        self->pid = pid;
        cork_pipe_close_write(&self->stdout_pipe);
        cork_pipe_close_write(&self->stderr_pipe);
        return 0;
    }
}

static void
cork_subprocess_terminate(struct cork_subprocess *self)
{
    if (self->pid > 0) {
        int  status;
        DEBUG("Terminating child process %d\n", (int) self->pid);
        kill(self->pid, SIGTERM);
        waitpid(self->pid, &status, 0);
        if (self->exit_code != NULL) {
            *self->exit_code = WEXITSTATUS(status);
        }
    }
}

static void
cork_subprocess_mark_terminated(struct cork_subprocess *self)
{
    self->pid = 0;
}


static struct cork_subprocess_group  *current_group;

static void
cork_subprocess_group_terminate(struct cork_subprocess_group *group)
{
    size_t  i;
    for (i = 0; i < cork_array_size(&current_group->subprocesses); i++) {
        struct cork_subprocess  *sub =
            cork_array_at(&current_group->subprocesses, i);
        cork_subprocess_terminate(sub);
    }
}

static struct cork_subprocess *
cork_subprocess_find(pid_t pid)
{
    size_t  i;
    for (i = 0; i < cork_array_size(&current_group->subprocesses); i++) {
        struct cork_subprocess  *sub =
            cork_array_at(&current_group->subprocesses, i);
        if (sub->pid == pid) {
            return sub;
        }
    }

    cork_unreachable();
}

static void
cork_subprocess_term_handler(int signum)
{
    size_t  i;
    DEBUG("++ Detected signal %d\n", signum);
    for (i = 0; i < cork_array_size(&current_group->subprocesses); i++) {
        struct cork_subprocess  *sub =
            cork_array_at(&current_group->subprocesses, i);
        if (sub->pid != 0) {
            DEBUG("++   Propagating signal to child %d\n", (int) sub->pid);
            kill(sub->pid, signum);
        }
    }
}

static void
cork_subprocess_chld_handler(int signum)
{
    pid_t  pid;
    int  status;

    DEBUG("++ Detected signal CHLD\n");
    while ((pid = waitpid(-1, &status, WNOHANG)) > 0) {
        struct cork_subprocess  *sub = cork_subprocess_find(pid);
        cork_subprocess_mark_terminated(sub);
        if (sub->exit_code != NULL) {
            *sub->exit_code = WEXITSTATUS(status);
        }
        current_group->still_running--;
        DEBUG("++   Processes still running: %zu\n",
              current_group->still_running);
    }
}

static void
cork_subprocess_install_handler(int signum, struct sigaction *old,
                                void (*handler)(int))
{
    struct sigaction  action;
    action.sa_handler = handler;
    sigemptyset(&action.sa_mask);
    action.sa_flags = 0;
    sigaction(signum, &action, old);
}

static void
cork_subprocess_install_term_handler(int signum, struct sigaction *old)
{
    sigaction(signum, NULL, old);
    if (old->sa_handler != SIG_IGN) {
        cork_subprocess_install_handler
            (signum, old, cork_subprocess_term_handler);
    }
}

static void
cork_subprocess_install_handlers(void)
{
    DEBUG("Installing signal handlers\n");
    cork_subprocess_install_term_handler(SIGHUP, &current_group->old_hup);
    cork_subprocess_install_term_handler(SIGINT, &current_group->old_int);
    cork_subprocess_install_term_handler(SIGTERM, &current_group->old_term);
    cork_subprocess_install_handler
        (SIGCHLD, &current_group->old_chld, cork_subprocess_chld_handler);
}

static void
cork_subprocess_restore_handlers(void)
{
    DEBUG("Restoring signal handlers\n");
    sigaction(SIGCHLD, &current_group->old_chld, NULL);
    sigaction(SIGHUP, &current_group->old_hup, NULL);
    sigaction(SIGINT, &current_group->old_int, NULL);
    sigaction(SIGTERM, &current_group->old_term, NULL);
}

static void
cork_subprocess_set_fds(fd_set *fds, int *nfds)
{
    size_t  i;
    DEBUG("Creating select(2) fd set\n");
    FD_ZERO(fds);
    for (i = 0; i < cork_array_size(&current_group->subprocesses); i++) {
        struct cork_subprocess  *sub =
            cork_array_at(&current_group->subprocesses, i);
        cork_pipe_set_fd(&sub->stdout_pipe, fds, nfds);
        cork_pipe_set_fd(&sub->stderr_pipe, fds, nfds);
    }
}

static int
cork_subprocess_read_fds(fd_set *fds)
{
    size_t  i;
    DEBUG("Reading from pipes\n");
    for (i = 0; i < cork_array_size(&current_group->subprocesses); i++) {
        struct cork_subprocess  *sub =
            cork_array_at(&current_group->subprocesses, i);
        rii_check(cork_pipe_read(&sub->stdout_pipe, current_group, fds));
        rii_check(cork_pipe_read(&sub->stderr_pipe, current_group, fds));
    }
    return 0;
}


int
cork_subprocess_group_start(struct cork_subprocess_group *group)
{
    size_t  i;

    if (current_group != NULL) {
        cork_error_set
            (CORK_BUILTIN_ERROR, CORK_SYSTEM_ERROR,
             "cork_subprocess_start is not thread-safe");
        return -1;
    }
    DEBUG("Starting subprocess group\n");
    current_group = group;

    /* Install signal handlers for a bunch of termination signals. */
    cork_subprocess_install_handlers();

    /* Start each subprocess. */
    for (i = 0; i < cork_array_size(&group->subprocesses); i++) {
        struct cork_subprocess  *sub =
            cork_array_at(&group->subprocesses, i);
        ei_check(cork_subprocess_fork(sub));
    }

    current_group->still_running = cork_array_size(&current_group->subprocesses);
    return 0;

error:
    cork_subprocess_group_terminate(group);
    cork_subprocess_restore_handlers();
    current_group = NULL;
    return -1;
}


int
cork_subprocess_group_abort(struct cork_subprocess_group *group)
{
    if (current_group == NULL) {
        /* Nothing is running; immediately return. */
        return 0;
    }

    assert(current_group == group);
    DEBUG("Aborting subprocess group\n");
    cork_subprocess_group_terminate(group);
    cork_subprocess_restore_handlers();
    current_group = NULL;
    return 0;
}


int
cork_subprocess_group_drain(struct cork_subprocess_group *group)
{
    int  nfds = 0;
    fd_set  fds;

    if (current_group == NULL) {
        /* Nothing is running; immediately return. */
        return 0;
    }

    assert(current_group == group);
    DEBUG("Draining subprocess group\n");
    cork_subprocess_set_fds(&fds, &nfds);

    /* Can't use our helper macro from above because we want to handle EINTR
     * specially. */
    DEBUG("Waiting for signal or readable pipes\n");
    if (select(nfds, &fds, NULL, NULL, NULL) == -1) {
        if (errno == EINTR) {
            DEBUG("  Interrupted by signal\n");
        } else {
            cork_system_error_set();
            goto error;
        }
    } else {
        ei_check(cork_subprocess_read_fds(&fds));
    }

    /* If there aren't any more processes running, clean up before returning. */
    if (group->still_running == 0) {
        cork_subprocess_restore_handlers();
        current_group = NULL;
    }
    return 0;

error:
    cork_subprocess_group_terminate(group);
    cork_subprocess_restore_handlers();
    current_group = NULL;
    return -1;
}

bool
cork_subprocess_group_is_finished(struct cork_subprocess_group *group)
{
    return (group->still_running == 0);
}

int
cork_subprocess_group_wait(struct cork_subprocess_group *group)
{
    if (current_group == NULL) {
        /* Nothing is running; immediately return. */
        return 0;
    }

    assert(current_group == group);
    DEBUG("Waiting for subprocess group to finish\n");
    while (group->still_running > 0) {
        rii_check(cork_subprocess_group_drain(group));
    }
    return 0;
}
