/* -*- coding: utf-8 -*-
 * ----------------------------------------------------------------------
 * Copyright © 2012, RedJack, LLC.
 * All rights reserved.
 *
 * Please see the COPYING file in this distribution for license
 * details.
 * ----------------------------------------------------------------------
 */

#ifndef LIBCORK_CORE_FILES_H
#define LIBCORK_CORE_FILES_H

#include <libcork/core/api.h>
#include <libcork/core/types.h>


#define CORK_SKIP_DIRECTORY  1

struct cork_dir_walker {
    int
    (*enter_directory)(struct cork_dir_walker *walker, const char *full_path,
                       const char *rel_path, const char *base_name);

    int
    (*file)(struct cork_dir_walker *walker, const char *full_path,
            const char *rel_path, const char *base_name);

    int
    (*leave_directory)(struct cork_dir_walker *walker, const char *full_path,
                       const char *rel_path, const char *base_name);
};

#define cork_dir_walker_enter_directory(w, fp, rp, bn) \
    ((w)->enter_directory((w), (fp), (rp), (bn)))

#define cork_dir_walker_file(w, fp, rp, bn) \
    ((w)->file((w), (fp), (rp), (bn)))

#define cork_dir_walker_leave_directory(w, fp, rp, bn) \
    ((w)->leave_directory((w), (fp), (rp), (bn)))


CORK_API int
cork_walk_directory(const char *path, struct cork_dir_walker *walker);


#endif /* LIBCORK_CORE_FILES_H */
