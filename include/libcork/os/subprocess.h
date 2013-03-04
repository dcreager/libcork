/* -*- coding: utf-8 -*-
 * ----------------------------------------------------------------------
 * Copyright © 2012-2013, RedJack, LLC.
 * All rights reserved.
 *
 * Please see the COPYING file in this distribution for license
 * details.
 * ----------------------------------------------------------------------
 */

#ifndef LIBCORK_OS_SUBPROCESS_H
#define LIBCORK_OS_SUBPROCESS_H

#include <stdarg.h>

#include <libcork/core/api.h>
#include <libcork/core/types.h>
#include <libcork/ds/stream.h>
#include <libcork/threads/basics.h>


/*-----------------------------------------------------------------------
 * Environments
 */

struct cork_env;

CORK_API struct cork_env *
cork_env_new(void);

CORK_API struct cork_env *
cork_env_clone_current(void);

CORK_API void
cork_env_free(struct cork_env *env);

CORK_API void
cork_env_add(struct cork_env *env, const char *name, const char *value);

CORK_API void
cork_env_add_printf(struct cork_env *env, const char *name,
                    const char *format, ...)
    CORK_ATTR_PRINTF(3,4);

CORK_API void
cork_env_add_vprintf(struct cork_env *env, const char *name,
                     const char *format, va_list args)
    CORK_ATTR_PRINTF(3,0);

CORK_API void
cork_env_remove(struct cork_env *env, const char *name);


CORK_API void
cork_env_replace_current(struct cork_env *env);


/*-----------------------------------------------------------------------
 * Subprocesses
 */

struct cork_subprocess;

/* If env is NULL, we use the environment variables of the calling process. */

/* Takes control of body and env */
CORK_API struct cork_subprocess *
cork_subprocess_new(struct cork_thread_body *body, struct cork_env *env,
                    struct cork_stream_consumer *stdout_consumer,
                    struct cork_stream_consumer *stderr_consumer,
                    int *exit_code);

/* Takes control of env */
CORK_API struct cork_subprocess *
cork_subprocess_new_exec(const char *program, char * const *params,
                         struct cork_env *env,
                         struct cork_stream_consumer *stdout_consumer,
                         struct cork_stream_consumer *stderr_consumer,
                         int *exit_code);

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


#endif /* LIBCORK_OS_SUBPROCESS_H */
