/* -*- coding: utf-8 -*-
 * ----------------------------------------------------------------------
 * Copyright © 2013, RedJack, LLC.
 * All rights reserved.
 *
 * Please see the COPYING file in this distribution for license details.
 * ----------------------------------------------------------------------
 */

#include <assert.h>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "libcork/core/attributes.h"
#include "libcork/core/error.h"
#include "libcork/core/types.h"
#include "libcork/ds/buffer.h"
#include "libcork/helpers/errors.h"
#include "libcork/helpers/posix.h"
#include "libcork/os/files.h"


/*-----------------------------------------------------------------------
 * Paths
 */

struct cork_path {
    struct cork_buffer  given;
};

static struct cork_path *
cork_path_new_internal(void)
{
    struct cork_path  *path = cork_new(struct cork_path);
    cork_buffer_init(&path->given);
    return path;
}

struct cork_path *
cork_path_new(const char *source)
{
    struct cork_path  *path = cork_path_new_internal();
    if (source == NULL) {
        cork_buffer_ensure_size(&path->given, 16);
        cork_buffer_set(&path->given, "", 0);
    } else {
        cork_buffer_set_string(&path->given, source);
    }
    return path;
}

struct cork_path *
cork_path_clone(const struct cork_path *other)
{
    struct cork_path  *path = cork_path_new_internal();
    cork_buffer_copy(&path->given, &other->given);
    return path;
}

void
cork_path_free(struct cork_path *path)
{
    cork_buffer_done(&path->given);
    free(path);
}

const char *
cork_path_get(const struct cork_path *path)
{
    return path->given.buf;
}


int
cork_path_set_absolute(struct cork_path *path)
{
    struct cork_buffer  buf;

    if (path->given.size > 0 &&
        cork_buffer_char(&path->given, path->given.size - 1) == '/') {
        /* The path is already absolute */
        return 0;
    }

    cork_buffer_init(&buf);
    cork_buffer_ensure_size(&buf, PATH_MAX);
    ep_check_posix(getcwd(buf.buf, PATH_MAX));
    buf.size = strlen(buf.buf);
    cork_buffer_append(&buf, "/", 1);
    cork_buffer_append_copy(&buf, &path->given);
    cork_buffer_done(&path->given);
    path->given = buf;
    return 0;

error:
    cork_buffer_done(&buf);
    return -1;
}

struct cork_path *
cork_path_absolute(const struct cork_path *other)
{
    struct cork_path  *path = cork_path_clone(other);
    cork_path_set_absolute(path);
    return path;
}


void
cork_path_append(struct cork_path *path, const char *more)
{
    if (more == NULL || more[0] == '\0') {
        return;
    }

    if (more[0] == '/') {
        /* If more starts with a "/", then its absolute, and should replace the
         * contents of the current path. */
        cork_buffer_set_string(&path->given, more);
    } else {
        /* Otherwise, more is relative, and should be appended to the current
         * path.  If the current given path doesn't end in a "/", then we need
         * to add one to keep the path well-formed. */

        if (path->given.size > 0 &&
            cork_buffer_char(&path->given, path->given.size - 1) != '/') {
            cork_buffer_append(&path->given, "/", 1);
        }

        cork_buffer_append_string(&path->given, more);
    }
}

struct cork_path *
cork_path_join(const struct cork_path *other, const char *more)
{
    struct cork_path  *path = cork_path_clone(other);
    cork_path_append(path, more);
    return path;
}

void
cork_path_append_path(struct cork_path *path, const struct cork_path *more)
{
    cork_path_append(path, more->given.buf);
}

struct cork_path *
cork_path_join_path(const struct cork_path *other, const struct cork_path *more)
{
    struct cork_path  *path = cork_path_clone(other);
    cork_path_append_path(path, more);
    return path;
}


void
cork_path_set_basename(struct cork_path *path)
{
    char  *given = path->given.buf;
    const char  *last_slash = strrchr(given, '/');
    if (last_slash != NULL) {
        size_t  offset = last_slash - given;
        size_t  basename_length = path->given.size - offset + 1;
        memmove(given, last_slash + 1, basename_length);
        given[basename_length] = '\0';
        path->given.size = basename_length;
    }
}

struct cork_path *
cork_path_basename(const struct cork_path *other)
{
    struct cork_path  *path = cork_path_clone(other);
    cork_path_set_basename(path);
    return path;
}


void
cork_path_set_dirname(struct cork_path *path)
{
    const char  *given = path->given.buf;
    const char  *last_slash = strrchr(given, '/');
    if (last_slash == NULL) {
        cork_buffer_clear(&path->given);
    } else {
        size_t  offset = last_slash - given;
        cork_buffer_truncate(&path->given, offset);
    }
}

struct cork_path *
cork_path_dirname(const struct cork_path *other)
{
    struct cork_path  *path = cork_path_clone(other);
    cork_path_set_dirname(path);
    return path;
}
