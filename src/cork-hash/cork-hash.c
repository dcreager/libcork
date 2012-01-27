/* -*- coding: utf-8 -*-
 * ----------------------------------------------------------------------
 * Copyright © 2012, RedJack, LLC.
 * All rights reserved.
 *
 * Please see the LICENSE.txt file in this distribution for license
 * details.
 * ----------------------------------------------------------------------
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <libcork/core.h>

int
main(int argc, char **argv)
{
    cork_hash  result;

    if (argc != 2) {
        fprintf(stderr, "Usage: cork-hash <string>\n");
        exit(EXIT_FAILURE);
    }

    /* don't include NUL terminator in hash */
    result = 0;
    result = cork_hash_buffer(result, argv[1], strlen(argv[1]));
    printf("0x%08" PRIx32 "\n", result);
    return EXIT_SUCCESS;
}
