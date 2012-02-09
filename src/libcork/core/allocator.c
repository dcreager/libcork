/* -*- coding: utf-8 -*-
 * ----------------------------------------------------------------------
 * Copyright © 2011, RedJack, LLC.
 * All rights reserved.
 *
 * Please see the LICENSE.txt file in this distribution for license
 * details.
 * ----------------------------------------------------------------------
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "libcork/core/allocator.h"
#include "libcork/core/error.h"
#include "libcork/core/types.h"


/*-----------------------------------------------------------------------
 * reallocf
 */

#if !CORK_HAVE_REALLOCF
void *
cork_realloc(void *ptr, size_t new_size)
{
    void  *result = realloc(ptr, new_size);
    if (result == NULL) {
        free(ptr);
    }
    return result;
}
#endif


/*-----------------------------------------------------------------------
 * Allocating strings
 */

const char *
cork_xstrdup(const char *str)
{
    size_t  len = strlen(str);
    size_t  allocated_size = len + sizeof(size_t) + 1;
    size_t  *new_str = malloc(allocated_size);
    if (new_str == NULL) {
        return NULL;
    }

    *new_str = allocated_size;
    char  *dest = (char *) (void *) (new_str + 1);
    strncpy(dest, str, len + 1);
    return dest;
}


void
cork_strfree(const char *str)
{
    size_t  *base = ((size_t *) str) - 1;
    free(base);
}
