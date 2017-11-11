/* -*- coding: utf-8 -*-
 * ----------------------------------------------------------------------
 * Copyright © 2012, libcork authors
 * All rights reserved.
 *
 * Please see the COPYING file in this distribution for license details.
 * ----------------------------------------------------------------------
 */

#include <stdio.h>

#include <libcork/core.h>

CORK_INITIALIZER(init)
{
    printf("Initializer 1\n");
}

CORK_FINALIZER(done)
{
    printf("Finalizer 1\n");
}
