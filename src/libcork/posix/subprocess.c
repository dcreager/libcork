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
#include "libcork/helpers/errors.h"


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
 * Pipes
 */

#define BUF_SIZE  4096
static char  read_buf[BUF_SIZE];

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

static void
cork_pipe_close_read(struct cork_pipe *p)
{
    if (p->fds[0] != -1) {
        close(p->fds[0]);
        p->fds[0] = -1;
    }
}

static void
cork_pipe_close_write(struct cork_pipe *p)
{
    if (p->fds[1] != -1) {
        close(p->fds[1]);
        p->fds[1] = -1;
    }
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
        ri_check_posix(pipe(p->fds));
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
        FD_SET(p->fds[0], fds);
        if (p->fds[0] >= *nfds) {
            *nfds = p->fds[0] + 1;
        }
    }
}

static int
cork_pipe_read(struct cork_pipe *p, fd_set *fds)
{
    if (p->fds[0] == -1 || !FD_ISSET(p->fds[0], fds)) {
        return 0;
    }

    do {
        ssize_t  bytes_read = read(p->fds[0], read_buf, BUF_SIZE);
        if (bytes_read == -1) {
            if (errno == EAGAIN) {
                /* We've exhausted all of the data currently available. */
                return 0;
            } else if (errno == EINTR) {
                /* Interrupted by a signal; return so that our wait loop can
                 * catch that. */
                return 0;
            } else {
                /* An actual error */
                cork_system_error_set();
                return -1;
            }
        } else if (bytes_read == 0) {
            rii_check(cork_stream_consumer_eof(p->consumer));
            close(p->fds[0]);
            p->fds[0] = -1;
            return 0;
        } else {
            rii_check(cork_stream_consumer_data
                      (p->consumer, read_buf, bytes_read, p->first));
            p->first = false;
        }
    } while (true);
}


/*-----------------------------------------------------------------------
 * Subprocesses
 */

struct cork_subprocess {
    const char  *program;
    char * const  *params;
    unsigned int  flags;
    pid_t  pid;
    struct cork_pipe  stdout_pipe;
    struct cork_pipe  stderr_pipe;
};


struct cork_subprocess *
cork_subprocess_new_exec(const char *program, char * const *params,
                         struct cork_stream_consumer *stdout_consumer,
                         struct cork_stream_consumer *stderr_consumer,
                         unsigned int flags)
{
    struct cork_subprocess  *self = cork_new(struct cork_subprocess);
    self->program = program;
    self->params = params;
    cork_pipe_init(&self->stdout_pipe, stdout_consumer);
    cork_pipe_init(&self->stderr_pipe, stderr_consumer);
    self->flags = flags;
    self->pid = 0;
    return self;
}

void
cork_subprocess_free(struct cork_subprocess *self)
{
    cork_pipe_done(&self->stdout_pipe);
    cork_pipe_done(&self->stderr_pipe);
    free(self);
}

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
    pid = fork();
    if (pid == 0) {
        /* Child process */

        /* Bind the stdout and stderr pipes */
        if (cork_pipe_dup(&self->stdout_pipe, STDOUT_FILENO) == -1) {
            _exit(EXIT_FAILURE);
        }
        if (cork_pipe_dup(&self->stderr_pipe, STDERR_FILENO) == -1) {
            _exit(EXIT_FAILURE);
        }

        /* Close the parent's end of the pipes */
        cork_pipe_close_read(&self->stdout_pipe);
        cork_pipe_close_read(&self->stderr_pipe);

        /* Execute the real program */
        execvp(self->program, self->params);

        /* If we fall through, there was an error execing the subprocess. */
        _exit(EXIT_FAILURE);
    } else if (pid < 0) {
        /* Error forking */
        cork_system_error_set();
        return -1;
    } else {
        /* Parent process */
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
        kill(self->pid, SIGTERM);
        waitpid(self->pid, NULL, 0);
    }
}

static void
cork_subprocess_mark_terminated(struct cork_subprocess *self)
{
    self->pid = 0;
}

static size_t  still_running;
static size_t  sub_count;
static struct cork_subprocess  **subs;
static struct sigaction  old_chld;
static struct sigaction  old_hup;
static struct sigaction  old_int;
static struct sigaction  old_term;

static struct cork_subprocess *
cork_subprocess_find(pid_t pid)
{
    size_t  i;
    for (i = 0; i < sub_count; i++) {
        if (subs[i]->pid == pid) {
            return subs[i];
        }
    }

    cork_unreachable();
}

static void
cork_subprocess_term_handler(int signum)
{
    size_t  i;
    for (i = 0; i < sub_count; i++) {
        if (subs[i]->pid != 0) {
            kill(subs[i]->pid, signum);
        }
    }
}

static void
cork_subprocess_chld_handler(int signum)
{
    pid_t  pid;
    int  status;

    if ((pid = wait(&status)) > 0) {
        struct cork_subprocess  *sub = cork_subprocess_find(pid);
        cork_subprocess_mark_terminated(sub);
        still_running--;
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
    cork_subprocess_install_term_handler(SIGHUP, &old_hup);
    cork_subprocess_install_term_handler(SIGINT, &old_int);
    cork_subprocess_install_term_handler(SIGTERM, &old_term);
    cork_subprocess_install_handler
        (SIGCHLD, &old_term, cork_subprocess_chld_handler);
}

static void
cork_subprocess_restore_handlers(void)
{
    sigaction(SIGCHLD, &old_chld, NULL);
    sigaction(SIGHUP, &old_hup, NULL);
    sigaction(SIGINT, &old_int, NULL);
    sigaction(SIGTERM, &old_term, NULL);
}

static void
cork_subprocess_set_fds(fd_set *fds, int *nfds)
{
    size_t  i;
    FD_ZERO(fds);
    for (i = 0; i < sub_count; i++) {
        if (subs[i]->pid > 0) {
            cork_pipe_set_fd(&subs[i]->stdout_pipe, fds, nfds);
            cork_pipe_set_fd(&subs[i]->stderr_pipe, fds, nfds);
        }
    }
}

static int
cork_subprocess_read_fds(fd_set *fds)
{
    size_t  i;
    for (i = 0; i < sub_count; i++) {
        if (subs[i]->pid > 0) {
            rii_check(cork_pipe_read(&subs[i]->stdout_pipe, fds));
            rii_check(cork_pipe_read(&subs[i]->stderr_pipe, fds));
        }
    }
    return 0;
}


static int
cork_subprocess_wait(void)
{
    still_running = sub_count;
    while (still_running > 0) {
        int  nfds = 0;
        fd_set  fds;
        cork_subprocess_set_fds(&fds, &nfds);
        /* Can't use our helper macro from above because we want to handle EINTR
         * specially.  (We want to repeat out outer loop, not the inner loop
         * inside the macro.) */
        if (select(nfds, &fds, NULL, NULL, NULL) == -1) {
            if (errno != EINTR) {
                cork_system_error_set();
                return -1;
            }
        } else {
            rii_check(cork_subprocess_read_fds(&fds));
        }
    }
    return 0;
}

int
cork_subprocess_start_and_wait(size_t _sub_count,
                               struct cork_subprocess **_subs)
{
    size_t  i;
    size_t  j;

    /* Install signal handlers for a bunch of termination signals. */
    sub_count = _sub_count;
    subs = _subs;
    cork_subprocess_install_handlers();

    /* Start each subprocess. */
    for (i = 0; i < _sub_count; i++) {
        ei_check(cork_subprocess_fork(_subs[i]));
    }

    /* Wait for them to finish. */
    ei_check(cork_subprocess_wait());

    /* Restore the signal handlers and return. */
    cork_subprocess_restore_handlers();
    return 0;

error:
    /* If there's an error, terminate any subprocesses that we already created. */
    for (j = 0; j < i; j++) {
        cork_subprocess_terminate(_subs[i]);
    }
    cork_subprocess_restore_handlers();
    return -1;
}
