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
 * @defgroup binary Binary data
 * @ingroup ds
 */

/**
 * @defgroup slice Slices
 * @ingroup binary
 */

/**
 * @defgroup managed_buffer Managed binary buffers
 * @ingroup binary
 */

/**
 * @defgroup buffer Resizable binary buffers
 * @ingroup binary
 */

/**
 * @defgroup stream Stream processing
 * @ingroup binary
 */

/**
 * @defgroup dllist Doubly-linked lists
 * @ingroup ds
 */

/*** include all of the parts ***/

#include <libcork/ds/buffer.h>
#include <libcork/ds/dllist.h>
#include <libcork/ds/managed-buffer.h>
#include <libcork/ds/slice.h>
#include <libcork/ds/stream.h>

#endif /* LIBCORK_DS_H */
