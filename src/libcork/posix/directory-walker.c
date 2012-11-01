/* -*- coding: utf-8 -*-
 * ----------------------------------------------------------------------
 * Copyright © 2012, RedJack, LLC.
 * All rights reserved.
 *
 * Please see the COPYING file in this distribution for license
 * details.
 * ----------------------------------------------------------------------
 */

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
#include "libcork/os/files.h"


static int
cork_walk_one_directory(struct cork_dir_walker *w, struct cork_buffer *path,
                        size_t root_path_size)
{
    int  rc;
    DIR  *dir;
    struct dirent  *entry;
    size_t  dir_path_size;

    dir = opendir(path->buf);
    if (CORK_UNLIKELY(dir == NULL)) {
        cork_system_error_set();
        return -1;
    }

    errno = 0;
    cork_buffer_append(path, "/", 1);
    dir_path_size = path->size;
    while ((entry = readdir(dir)) != NULL) {
        struct stat  info;

        /* Skip the "." and ".." entries */
        if (strcmp(entry->d_name, ".") == 0 ||
            strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        /* Stat the directory entry */
        cork_buffer_append_string(path, entry->d_name);

        rc = stat(path->buf, &info);
        if (CORK_UNLIKELY(rc == -1)) {
            cork_system_error_set();
            closedir(dir);
            return -1;
        }

        /* If the entry is a subdirectory, recurse into it. */
        if (S_ISDIR(info.st_mode)) {
            int  rc = cork_dir_walker_enter_directory
                (w, path->buf, path->buf + root_path_size,
                 path->buf + dir_path_size);
            if (rc != CORK_SKIP_DIRECTORY) {
                rii_check(cork_walk_one_directory(w, path, root_path_size));
                rii_check(cork_dir_walker_leave_directory
                          (w, path->buf, path->buf + root_path_size,
                           path->buf + dir_path_size));
            }
        } else if (S_ISREG(info.st_mode)) {
            rii_check(cork_dir_walker_file
                      (w, path->buf, path->buf + root_path_size,
                       path->buf + dir_path_size));
        }

        /* Remove this entry name from the path buffer. */
        cork_buffer_truncate(path, dir_path_size);
    }

    /* Remove the trailing '/' from the path buffer. */
    cork_buffer_truncate(path, dir_path_size - 1);
    closedir(dir);

    if (CORK_UNLIKELY(errno != 0)) {
        cork_system_error_set();
        return -1;
    }

    return 0;
}

int
cork_walk_directory(const char *path, struct cork_dir_walker *w)
{
    int  rc;
    char  *p;
    struct cork_buffer  buf = CORK_BUFFER_INIT();

    /* Seed the buffer with the directory's path, ensuring that there's no
     * trailing '/' */
    cork_buffer_append_string(&buf, path);
    p = buf.buf;
    while (p[buf.size-1] == '/') {
        buf.size--;
        p[buf.size] = '\0';
    }

    rc = cork_walk_one_directory(w, &buf, buf.size + 1);
    cork_buffer_done(&buf);
    return rc;
}
