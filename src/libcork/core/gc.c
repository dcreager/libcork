/* -*- coding: utf-8 -*-
 * ----------------------------------------------------------------------
 * Copyright © 2011, RedJack, LLC.
 * All rights reserved.
 *
 * Please see the LICENSE.txt file in this distribution for license
 * details.
 * ----------------------------------------------------------------------
 */

#include "libcork/core/allocator.h"
#include "libcork/core/gc.h"
#include "libcork/core/types.h"
#include "libcork/ds/dllist.h"


#if !defined(CORK_DEBUG_GC)
#define CORK_DEBUG_GC  0
#endif

#if CORK_DEBUG_GC
#include <stdio.h>
#define DEBUG(...) fprintf(stderr, __VA_ARGS__)
#else
#define DEBUG(...) /* no debug messages */
#endif


struct cork_gc_header {
    /**
     * @brief The current reference count for this object, along with
     * its color during the mark/sweep process.
     * @private
     */
    volatile int  ref_count_color;

    /**
     * @brief The allocated size of this garbage-collected object
     * (including the header)
     * @private
     */
    size_t  allocated_size;

    /**
     * @brief The garbage collection context that this object belongs
     * to.
     * @private
     */
    struct cork_gc  *gc;

    /**
     * @brief The garbage collection interface for this object.
     * @private
     */
    struct cork_gc_obj_iface  *iface;
};

/*
 * Structure of ref_count_color:
 *
 *   +-----+---+---+---+---+---+
 *   | ... | 4 | 3 | 2 | 1 | 0 |
 *   +-----+---+---+---+---+---+
 *      ref_count    |   color
 *                   |
 *        buffered --/
 */

#define cork_gc_ref_count_color(count, buffered, color) \
    (((count) << 3) | ((buffered) << 2) | (color))

#define cork_gc_get_ref_count(hdr) \
    ((hdr)->ref_count_color >> 3)

#define cork_gc_inc_ref_count(hdr) \
    do { \
        (hdr)->ref_count_color += (1 << 3); \
    } while (0)

#define cork_gc_dec_ref_count(hdr) \
    do { \
        (hdr)->ref_count_color -= (1 << 3); \
    } while (0)

#define cork_gc_get_color(hdr) \
    ((hdr)->ref_count_color & 0x3)

#define cork_gc_set_color(hdr, color) \
    do { \
        (hdr)->ref_count_color = \
            ((hdr)->ref_count_color & ~0x3) | (color & 0x3); \
    } while (0)

#define cork_gc_get_buffered(hdr) \
    (((hdr)->ref_count_color & 0x4) != 0)

#define cork_gc_set_buffered(hdr, buffered) \
    do { \
        (hdr)->ref_count_color = \
            ((hdr)->ref_count_color & ~0x4) | (((buffered) & 1) << 2); \
    } while (0)

#define cork_gc_free(hdr) \
    do { \
        if ((hdr)->iface->free != NULL) { \
            (hdr)->iface->free((hdr)->gc, cork_gc_get_object((hdr))); \
        } \
        cork_free((hdr)->gc->alloc, (hdr), (hdr)->allocated_size); \
    } while (0)

#define cork_gc_recurse(hdr, recurser) \
    do { \
        if ((hdr)->iface->recurse != NULL) { \
            (hdr)->iface->recurse \
                (cork_gc_get_object((hdr)), (recurser), NULL); \
        } \
    } while (0)

enum cork_gc_color {
    /** @brief In use or free */
    GC_BLACK = 0,
    /** @brief Possible member of garbage cycle */
    GC_GRAY = 1,
    /** @brief Member of garbage cycle */
    GC_WHITE = 2,
    /** @brief Possible root of garbage cycle */
    GC_PURPLE = 3
};

#define cork_gc_get_header(obj) \
    (((struct cork_gc_header *) (obj)) - 1)

#define cork_gc_get_object(hdr) \
    ((void *) (((struct cork_gc_header *) (hdr)) + 1))


#define ROOTS_SIZE  1024

static void
cork_gc_collect_cycles(struct cork_gc *gc);

int
cork_gc_init(struct cork_gc *gc, struct cork_alloc *alloc)
{
    gc->roots =
        cork_malloc(alloc, sizeof(struct cork_gc_header *) * ROOTS_SIZE);
    if (gc->roots == NULL) {
        return 1;
    }

    gc->alloc = alloc;
    gc->root_count = 0;
    return 0;
}

void
cork_gc_done(struct cork_gc *gc)
{
    cork_gc_collect_cycles(gc);
    cork_free(gc->alloc, gc->roots,
              sizeof(struct cork_gc_header *) * ROOTS_SIZE);
    gc->alloc = NULL;
    gc->roots = NULL;
    gc->root_count = 0;
}

void *
cork_gc_alloc(struct cork_gc *gc, size_t instance_size,
              struct cork_gc_obj_iface *iface)
{
    size_t  full_size = instance_size + sizeof(struct cork_gc_header);
    DEBUG("Allocating %zu (%zu) bytes\n", instance_size, full_size);
    struct cork_gc_header  *header = cork_malloc(gc->alloc, full_size);
    if (header == NULL) {
        return NULL;
    }

    DEBUG("  Result is %p[%p]\n", cork_gc_get_object(header), header);
    header->ref_count_color = cork_gc_ref_count_color(1, false, GC_BLACK);
    header->allocated_size = full_size;
    header->gc = gc;
    header->iface = iface;
    return cork_gc_get_object(header);
}

void *
cork_gc_incref(struct cork_gc *gc, void *obj)
{
    if (obj != NULL) {
        struct cork_gc_header  *header = cork_gc_get_header(obj);
        cork_gc_inc_ref_count(header);
        DEBUG("Incrementing %p -> %d\n",
              obj, cork_gc_get_ref_count(header));
        cork_gc_set_color(header, GC_BLACK);
    }
    return obj;
}

static void
cork_gc_decref_step(void *obj, void *ud);

static void
cork_gc_release(struct cork_gc_header *header)
{
    cork_gc_recurse(header, cork_gc_decref_step);
    cork_gc_set_color(header, GC_BLACK);
    if (!cork_gc_get_buffered(header)) {
        cork_gc_free(header);
    }
}

static void
cork_gc_possible_root(struct cork_gc_header *header)
{
    if (cork_gc_get_color(header) != GC_PURPLE) {
        DEBUG("  Possible garbage cycle root\n");
        cork_gc_set_color(header, GC_PURPLE);
        if (!cork_gc_get_buffered(header)) {
            cork_gc_set_buffered(header, true);
            if (header->gc->root_count >= ROOTS_SIZE) {
                cork_gc_collect_cycles(header->gc);
            }
            header->gc->roots[header->gc->root_count++] = header;
        }
    } else {
        DEBUG("  Already marked as possible garbage cycle root\n");
    }
}

static void
cork_gc_decref_step(void *obj, void *ud)
{
    if (obj != NULL) {
        struct cork_gc_header  *header = cork_gc_get_header(obj);
        cork_gc_dec_ref_count(header);
        DEBUG("Decrementing %p -> %d\n",
              obj, cork_gc_get_ref_count(header));
        if (cork_gc_get_ref_count(header) == 0) {
            DEBUG("  Releasing %p\n", header);
            cork_gc_release(header);
        } else {
            cork_gc_possible_root(header);
        }
    }
}

void
cork_gc_decref(struct cork_gc *gc, void *obj)
{
    cork_gc_decref_step(obj, NULL);
}


static void
cork_gc_mark_gray_step(void *obj, void *ud);

static void
cork_gc_mark_gray(struct cork_gc_header *header)
{
    if (cork_gc_get_color(header) != GC_GRAY) {
        DEBUG("      Setting color to gray\n");
        cork_gc_set_color(header, GC_GRAY);
        cork_gc_recurse(header, cork_gc_mark_gray_step);
    }
}

static void
cork_gc_mark_gray_step(void *obj, void *ud)
{
    if (obj != NULL) {
        DEBUG("    cork_gc_mark_gray(%p)\n", obj);
        struct cork_gc_header  *header = cork_gc_get_header(obj);
        cork_gc_dec_ref_count(header);
        DEBUG("      Reference count now %d\n", cork_gc_get_ref_count(header));
        cork_gc_mark_gray(header);
    }
}

static void
cork_gc_mark_roots(struct cork_gc *gc)
{
    size_t  i;
    for (i = 0; i < gc->root_count; i++) {
        struct cork_gc_header  *header = gc->roots[i];
        if (cork_gc_get_color(header) == GC_PURPLE) {
            DEBUG("  Checking possible garbage cycle root %p\n",
                  cork_gc_get_object(header));
            DEBUG("    cork_gc_mark_gray(%p)\n",
                  cork_gc_get_object(header));
            cork_gc_mark_gray(header);
        } else {
            DEBUG("  Possible garbage cycle root %p already checked\n",
                  cork_gc_get_object(header));
            cork_gc_set_buffered(header, false);
            gc->roots[i] = NULL;
            if (cork_gc_get_color(header) == GC_BLACK &&
                cork_gc_get_ref_count(header) == 0) {
                DEBUG("  Freeing %p\n", header);
                cork_gc_free(header);
            }
        }
    }
}

static void
cork_gc_scan_black_step(void *obj, void *ud);

static void
cork_gc_scan_black(struct cork_gc_header *header)
{
    DEBUG("      Setting color of %p to BLACK\n",
          cork_gc_get_object(header));
    cork_gc_set_color(header, GC_BLACK);
    cork_gc_recurse(header, cork_gc_scan_black_step);
}

static void
cork_gc_scan_black_step(void *obj, void *ud)
{
    if (obj != NULL) {
        struct cork_gc_header  *header = cork_gc_get_header(obj);
        cork_gc_inc_ref_count(header);
        DEBUG("      Increasing reference count %p -> %d\n",
              obj, cork_gc_get_ref_count(header));
        if (cork_gc_get_color(header) != GC_BLACK) {
            cork_gc_scan_black(header);
        }
    }
}

static void
cork_gc_scan(void *obj, void *ud)
{
    if (obj != NULL) {
        DEBUG("  Scanning possible garbage cycle entry %p\n", obj);
        struct cork_gc_header  *header = cork_gc_get_header(obj);
        if (cork_gc_get_color(header) == GC_GRAY) {
            if (cork_gc_get_ref_count(header) > 0) {
                DEBUG("    Remaining references; can't be a cycle\n");
                cork_gc_scan_black(header);
            } else {
                DEBUG("    Definitely a garbage cycle\n");
                cork_gc_set_color(header, GC_WHITE);
                cork_gc_recurse(header, cork_gc_scan);
            }
        } else {
            DEBUG("    Already checked\n");
        }
    }
}

static void
cork_gc_scan_roots(struct cork_gc *gc)
{
    size_t  i;
    for (i = 0; i < gc->root_count; i++) {
        if (gc->roots[i] != NULL) {
            void  *obj = cork_gc_get_object(gc->roots[i]);
            cork_gc_scan(obj, NULL);
        }
    }
}

static void
cork_gc_collect_white(void *obj, void *ud)
{
    if (obj != NULL) {
        struct cork_gc_header  *header = cork_gc_get_header(obj);
        if (cork_gc_get_color(header) == GC_WHITE &&
            !cork_gc_get_buffered(header)) {
            DEBUG("  Releasing %p\n", obj);
            cork_gc_set_color(header, GC_BLACK);
            cork_gc_recurse(header, cork_gc_collect_white);
            DEBUG("  Freeing %p\n", header);
            cork_gc_free(header);
        }
    }
}

static void
cork_gc_collect_roots(struct cork_gc *gc)
{
    size_t  i;
    for (i = 0; i < gc->root_count; i++) {
        if (gc->roots[i] != NULL) {
            struct cork_gc_header  *header = gc->roots[i];
            void  *obj = cork_gc_get_object(header);
            cork_gc_set_buffered(header, false);
            DEBUG("Collecting cycles from garbage root %p\n", obj);
            cork_gc_collect_white(obj, NULL);
            gc->roots[i] = NULL;
        }
    }
    gc->root_count = 0;
}

static void
cork_gc_collect_cycles(struct cork_gc *gc)
{
    DEBUG("Collecting garbage cycles\n");
    cork_gc_mark_roots(gc);
    cork_gc_scan_roots(gc);
    cork_gc_collect_roots(gc);
}
