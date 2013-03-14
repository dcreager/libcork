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
 * Files
 */

#define CORK_FILE_RECURSIVE   0x0001
#define CORK_FILE_PERMISSIVE  0x0002

typedef unsigned int  cork_file_mode;

enum cork_file_type {
    CORK_FILE_MISSING = 0,
    CORK_FILE_REGULAR = 1,
    CORK_FILE_DIRECTORY = 2,
    CORK_FILE_SYMLINK = 3,
    CORK_FILE_UNKNOWN = 4
};

struct cork_file;

CORK_API struct cork_file *
cork_file_new(const char *path);

/* Takes control of path */
CORK_API struct cork_file *
cork_file_new_from_path(struct cork_path *path);

CORK_API void
cork_file_free(struct cork_file *file);

/* File owns the result; you should not free it */
CORK_API const struct cork_path *
cork_file_path(struct cork_file *file);

CORK_API int
cork_file_exists(struct cork_file *file, bool *exists);

CORK_API int
cork_file_type(struct cork_file *file, enum cork_file_type *type);


typedef int
(*cork_file_directory_iterator)(struct cork_file *child, const char *rel_name,
                                void *user_data);

CORK_API int
cork_file_iterate_directory(struct cork_file *file,
                            cork_file_directory_iterator iterator,
                            void *user_data);

/* If flags includes CORK_FILE_RECURSIVE, this creates parent directories,
 * if needed.  If flags doesn't include CORK_FILE_PERMISSIVE, then it's an error
 * if the directory already exists. */
CORK_API int
cork_file_mkdir(struct cork_file *file, cork_file_mode mode,
                unsigned int flags);

/* Removes a file or directory.  If file is a directory, and flags contains
 * CORK_FILE_RECURSIVE, then all of the directory's contents are removed, too.
 * Otherwise, the directory must already be empty. */
CORK_API int
cork_file_remove(struct cork_file *file, unsigned int flags);


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
