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

#include <libcork/core/api.h>
#include <libcork/core/types.h>
#include <libcork/ds/stream.h>
#include <libcork/threads/basics.h>


/*-----------------------------------------------------------------------
 * Subprocesses
 */

struct cork_subprocess;

/* Takes control of body */
CORK_API struct cork_subprocess *
cork_subprocess_new(struct cork_thread_body *body,
                    struct cork_stream_consumer *stdout_consumer,
                    struct cork_stream_consumer *stderr_consumer);

CORK_API struct cork_subprocess *
cork_subprocess_new_exec(const char *program, char * const *params,
                         struct cork_stream_consumer *stdout_consumer,
                         struct cork_stream_consumer *stderr_consumer);

CORK_API void
cork_subprocess_free(struct cork_subprocess *sub);


/*-----------------------------------------------------------------------
 * Groups of subprocesses
 */

struct cork_subprocess_group;

CORK_API struct cork_subprocess_group *
cork_subprocess_group_new(void);

CORK_API void
cork_subprocess_group_free(struct cork_subprocess_group *group);

/* Takes control of sub */
CORK_API void
cork_subprocess_group_add(struct cork_subprocess_group *group,
                          struct cork_subprocess *sub);

CORK_API int
cork_subprocess_group_start(struct cork_subprocess_group *group);

CORK_API bool
cork_subprocess_group_is_finished(struct cork_subprocess_group *group);

CORK_API int
cork_subprocess_group_abort(struct cork_subprocess_group *group);

CORK_API int
cork_subprocess_group_drain(struct cork_subprocess_group *group);

CORK_API int
cork_subprocess_group_wait(struct cork_subprocess_group *group);


#endif /* LIBCORK_OS_PROCESS_H */
