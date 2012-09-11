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
#include <signal.h>
#include <sys/wait.h>
#include <unistd.h>

#include "libcork/core.h"
#include "libcork/ds.h"
#include "libcork/helpers/errors.h"


struct cork_subprocess {
    const char  *program;
    char * const  *params;
    struct cork_stream_consumer  *stdout_consumer;
    struct cork_stream_consumer  *stderr_consumer;
    unsigned int  flags;
    pid_t  pid;
};


struct cork_subprocess *
cork_subprocess_new_exec(const char *program, char * const *params,
                         struct cork_stream_consumer *stdout_consumer,
                         struct cork_stream_consumer *stderr_consumer,
                         unsigned int flags)
{
    assert(stdout_consumer == NULL);
    assert(stderr_consumer == NULL);
    struct cork_subprocess  *self = cork_new(struct cork_subprocess);
    self->program = program;
    self->params = params;
    self->stdout_consumer = stdout_consumer;
    self->stderr_consumer = stderr_consumer;
    self->flags = flags;
    self->pid = 0;
    return self;
}

void
cork_subprocess_free(struct cork_subprocess *self)
{
    free(self);
}

static int
cork_subprocess_fork(struct cork_subprocess *self)
{
    pid_t  pid = fork();

    if (pid == 0) {
        /* Child process */
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

static struct cork_subprocess *
cork_subprocess_find(pid_t pid, size_t sub_count, struct cork_subprocess **subs)
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
cork_subprocess_mark_terminated(struct cork_subprocess *self)
{
    self->pid = 0;
}

static size_t  sub_count;
static struct cork_subprocess  **subs;
static struct sigaction  old_hup;
static struct sigaction  old_int;
static struct sigaction  old_term;

static void
cork_subprocess_signal_handler(int signum)
{
    size_t  i;
    for (i = 0; i < sub_count; i++) {
        if (subs[i]->pid != 0) {
            kill(subs[i]->pid, signum);
        }
    }
}

static void
cork_subprocess_install_one_handler(int signum, struct sigaction *old)
{
    sigaction(signum, NULL, old);
    if (old->sa_handler != SIG_IGN) {
        struct sigaction  action;
        action.sa_handler = cork_subprocess_signal_handler;
        sigemptyset(&action.sa_mask);
        action.sa_flags = 0;
        sigaction(signum, &action, NULL);
    }
}

static void
cork_subprocess_install_handlers(void)
{
    cork_subprocess_install_one_handler(SIGHUP, &old_hup);
    cork_subprocess_install_one_handler(SIGINT, &old_int);
    cork_subprocess_install_one_handler(SIGTERM, &old_term);
}

static void
cork_subprocess_restore_handlers(void)
{
    sigaction(SIGHUP, &old_hup, NULL);
    sigaction(SIGINT, &old_int, NULL);
    sigaction(SIGTERM, &old_term, NULL);
}

static int
cork_subprocess_wait(size_t sub_count, struct cork_subprocess **subs)
{
    size_t  terminated_count = 0;

    while (terminated_count < sub_count) {
        int  status;
        pid_t  pid = waitpid(-1, &status, 0);

        if (pid < 0) {
            cork_system_error_set();
        } else {
            struct cork_subprocess  *sub =
                cork_subprocess_find(pid, sub_count, subs);
            cork_subprocess_mark_terminated(sub);
            terminated_count++;
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
    ei_check(cork_subprocess_wait(_sub_count, _subs));

    /* Restore the signal handlers. */
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
