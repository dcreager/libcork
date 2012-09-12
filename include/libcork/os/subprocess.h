/* -*- coding: utf-8 -*-
 * ----------------------------------------------------------------------
 * Copyright © 2012, RedJack, LLC.
 * All rights reserved.
 *
 * Please see the COPYING file in this distribution for license
 * details.
 * ----------------------------------------------------------------------
 */

#ifndef LIBCORK_OS_PROCESS_H
#define LIBCORK_OS_PROCESS_H

#include <libcork/core/types.h>
#include <libcork/ds/stream.h>
#include <libcork/threads/basics.h>


/*-----------------------------------------------------------------------
 * Subprocesses
 */

struct cork_subprocess;

/* Takes control of body */
struct cork_subprocess *
cork_subprocess_new(struct cork_thread_body *body,
                    struct cork_stream_consumer *stdout_consumer,
                    struct cork_stream_consumer *stderr_consumer);

struct cork_subprocess *
cork_subprocess_new_exec(const char *program, char * const *params,
                         struct cork_stream_consumer *stdout_consumer,
                         struct cork_stream_consumer *stderr_consumer);

void
cork_subprocess_free(struct cork_subprocess *sub);


/*-----------------------------------------------------------------------
 * Groups of subprocesses
 */

struct cork_subprocess_group;

struct cork_subprocess_group *
cork_subprocess_group_new(void);

void
cork_subprocess_group_free(struct cork_subprocess_group *group);

/* Takes control of sub */
void
cork_subprocess_group_add(struct cork_subprocess_group *group,
                          struct cork_subprocess *sub);

int
cork_subprocess_group_start(struct cork_subprocess_group *group);

bool
cork_subprocess_group_is_finished(struct cork_subprocess_group *group);

int
cork_subprocess_group_abort(struct cork_subprocess_group *group);

int
cork_subprocess_group_drain(struct cork_subprocess_group *group);

int
cork_subprocess_group_wait(struct cork_subprocess_group *group);


#endif /* LIBCORK_OS_PROCESS_H */
