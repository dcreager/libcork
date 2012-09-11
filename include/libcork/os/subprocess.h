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

#include <libcork/core.h>
#include <libcork/ds.h>


struct cork_subprocess;


struct cork_subprocess *
cork_subprocess_new_exec(const char *program, char * const *params,
                         struct cork_stream_consumer *stdout_consumer,
                         struct cork_stream_consumer *stderr_consumer,
                         unsigned int flags);

void
cork_subprocess_free(struct cork_subprocess *sub);

int
cork_subprocess_start_and_wait(size_t sub_count, struct cork_subprocess **subs);


#endif /* LIBCORK_OS_PROCESS_H */
