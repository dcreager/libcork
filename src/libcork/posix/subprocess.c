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
cork_subprocess_start_and_wait(size_t sub_count, struct cork_subprocess **subs)
{
    size_t  i;
    size_t  j;

    for (i = 0; i < sub_count; i++) {
        ei_check(cork_subprocess_fork(subs[i]));
    }

    ei_check(cork_subprocess_wait(sub_count, subs));
    return 0;

error:
    /* If there's an error, terminate any subprocesses that we already created. */
    for (j = 0; j < i; j++) {
        cork_subprocess_terminate(subs[i]);
    }
    return -1;
}
