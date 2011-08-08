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
#include "libcork/core/types.h"
#include "libcork/ds/dllist.h"


#if !defined(CORK_DEBUG_HALLOC)
#define CORK_DEBUG_HALLOC  0
#endif

#if CORK_DEBUG_HALLOC
#include <stdio.h>
#define DEBUG(...) fprintf(stderr, __VA_ARGS__)
#else
#define DEBUG(...) /* no debug messages */
#endif


/**
 * @brief The header of each object in a hierarchical memory tree.
 * @since 0.2
 */

typedef struct cork_halloc_header_t  cork_halloc_header_t;

struct cork_halloc_header_t {
    cork_dllist_item_t  siblings;
    cork_dllist_t  children;
    cork_halloc_header_t  *parent;
    cork_allocator_t  *allocator;
    cork_halloc_destructor_t  destructor;
    size_t  size;
};

#define cork_halloc_header_from_ptr(ptr) \
    ((cork_halloc_header_t *) \
     (((char *) (ptr)) - sizeof(cork_halloc_header_t)))

#define cork_halloc_ptr_from_header(header) \
    ((void *) \
     (((char *) (header)) + sizeof(cork_halloc_header_t)))


void *
cork_halloc_new_root(cork_allocator_t *alloc)
{
    size_t  real_size = sizeof(cork_halloc_header_t);
    DEBUG("---\nAllocating root (size=%zu)\n", real_size);
    cork_halloc_header_t  *header = cork_malloc(alloc, real_size);
    if (header == NULL) {
        return NULL;
    }
    DEBUG("Root   is %p[%p]\n", cork_halloc_ptr_from_header(header), header);

    header->siblings.next = NULL;
    header->siblings.prev = NULL;
    cork_dllist_init(&header->children);
    header->parent = NULL;
    header->allocator = alloc;
    header->destructor = NULL;
    header->size = real_size;
    return cork_halloc_ptr_from_header(header);
}


void *
cork_halloc_malloc(cork_halloc_t *vparent, size_t size)
{
    if (vparent == NULL) {
        return NULL;
    }

    cork_halloc_header_t  *parent = cork_halloc_header_from_ptr(vparent);
    DEBUG("---\nParent is %p[%p]\n", vparent, parent);
    size_t  real_size = size + sizeof(cork_halloc_header_t);
    DEBUG("Allocating child (size=%zu)\n", real_size);
    cork_halloc_header_t  *header = cork_malloc(parent->allocator, real_size);
    if (header == NULL) {
        return NULL;
    }
    DEBUG("Child  is %p[%p]\n", cork_halloc_ptr_from_header(header), header);

    cork_dllist_add(&parent->children, &header->siblings);
    cork_dllist_init(&header->children);
    header->parent = parent;
    header->allocator = parent->allocator;
    header->destructor = NULL;
    header->size = real_size;
    return cork_halloc_ptr_from_header(header);
}


static void
cork_halloc_set_new_parent(cork_dllist_item_t *vchild, void *user_data)
{
    cork_halloc_header_t  *child =
        cork_container_of(vchild, cork_halloc_header_t, siblings);
    child->parent = user_data;
}

void *
cork_halloc_realloc(cork_halloc_t *ptr, size_t nsize)
{
    cork_halloc_header_t  *header = cork_halloc_header_from_ptr(ptr);
    DEBUG("---\nObject is %p[%p]\n", ptr, header);
    size_t  real_nsize = nsize + sizeof(cork_halloc_header_t);
    DEBUG("Reallocating [new size=%zu]\n", real_nsize);
    cork_halloc_header_t  *new_header =
        cork_realloc(header->allocator, header, header->size, real_nsize);
    if (new_header == NULL) {
        cork_halloc_free(ptr);
        return NULL;
    }
    DEBUG("Newobj is %p[%p]\n",
          cork_halloc_ptr_from_header(new_header), new_header);

    if (header != new_header) {
        /* The pointer moved, so we have to update the lists. */
        new_header->siblings.prev->next = &new_header->siblings;
        new_header->siblings.next->prev = &new_header->siblings;
        new_header->children.head.next->prev = &new_header->children.head;
        new_header->children.head.prev->next = &new_header->children.head;
        cork_dllist_map(&new_header->children,
                        cork_halloc_set_new_parent, new_header);
    }

    new_header->size = real_nsize;
    return cork_halloc_ptr_from_header(new_header);
}


void
cork_halloc_set_destructor(cork_halloc_t *ptr,
                           cork_halloc_destructor_t destructor)
{
    cork_halloc_header_t  *header = cork_halloc_header_from_ptr(ptr);
    header->destructor = destructor;
}


static void
cork_halloc_free_child(cork_dllist_item_t *vchild, void *user_data)
{
    cork_halloc_header_t  *child =
        cork_container_of(vchild, cork_halloc_header_t, siblings);
    cork_halloc_t  *child_ptr = cork_halloc_ptr_from_header(child);
    cork_halloc_free(child_ptr);
}

void
cork_halloc_free(cork_halloc_t *ptr)
{
    cork_halloc_header_t  *header = cork_halloc_header_from_ptr(ptr);
    if (header->destructor != NULL) {
        header->destructor(ptr);
        header->destructor = NULL;
    }
    cork_dllist_map(&header->children, cork_halloc_free_child, NULL);
    cork_free(header->allocator, header, header->size);
}
