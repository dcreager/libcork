/* -*- coding: utf-8 -*-
 * ----------------------------------------------------------------------
 * Copyright © 2011, RedJack, LLC.
 * All rights reserved.
 *
 * Please see the LICENSE.txt file in this distribution for license
 * details.
 * ----------------------------------------------------------------------
 */

#ifndef LIBCORK_DS_H
#define LIBCORK_DS_H

/**
 * @file
 * @brief Pulls in all features of the @ref ds
 */

/**
 * @defgroup ds Data structures module
 *
 * <tt>#%include \<libcork/ds.h\></tt>
 */

/*** order of groups in documentation ***/

/**
 * @defgroup dllist Doubly-linked lists
 * @ingroup ds
 */

/**
 * @defgroup managed_buffer Managed binary buffers
 * @ingroup ds
 */

/*** include all of the parts ***/

#include <libcork/ds/dllist.h>
#include <libcork/ds/managed-buffer.h>

#endif /* LIBCORK_DS_H */
