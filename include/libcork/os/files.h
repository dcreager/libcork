/* -*- coding: utf-8 -*-
 * ----------------------------------------------------------------------
 * Copyright © 2012-2013, RedJack, LLC.
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


/*-----------------------------------------------------------------------
 * Paths
 */

struct cork_path;

/* path can be relative or absolute */
CORK_API struct cork_path *
cork_path_new(const char *path);

CORK_API struct cork_path *
cork_path_clone(const struct cork_path *other);

CORK_API void
cork_path_free(struct cork_path *path);

CORK_API const char *
cork_path_get(const struct cork_path *path);


CORK_API int
cork_path_set_absolute(struct cork_path *path);

CORK_API struct cork_path *
cork_path_absolute(const struct cork_path *other);


CORK_API void
cork_path_append(struct cork_path *path, const char *more);

CORK_API void
cork_path_append_path(struct cork_path *path, const struct cork_path *more);

CORK_API struct cork_path *
cork_path_join(const struct cork_path *other, const char *more);

CORK_API struct cork_path *
cork_path_join_path(const struct cork_path *other,
                    const struct cork_path *more);


CORK_API void
cork_path_set_basename(struct cork_path *path);

CORK_API struct cork_path *
cork_path_basename(const struct cork_path *other);


CORK_API void
cork_path_set_dirname(struct cork_path *path);

CORK_API struct cork_path *
cork_path_dirname(const struct cork_path *other);


/*-----------------------------------------------------------------------
 * Walking a directory tree
 */

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
