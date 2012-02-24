/* -*- coding: utf-8 -*-
 * ----------------------------------------------------------------------
 * Copyright © 2011, RedJack, LLC.
 * All rights reserved.
 *
 * Please see the LICENSE.txt file in this distribution for license
 * details.
 * ----------------------------------------------------------------------
 */

#ifndef LIBCORK_DS_DLLIST_H
#define LIBCORK_DS_DLLIST_H


#include <libcork/core/types.h>


struct cork_dllist_item {
    /* A pointer to the next element in the list. */
    struct cork_dllist_item  *next;
    /* A pointer to the previous element in the list. */
    struct cork_dllist_item  *prev;
};


struct cork_dllist {
    /* The sentinel element for this list. */
    struct cork_dllist_item  head;
};


void
cork_dllist_init(struct cork_dllist *list);


typedef void
(*cork_dllist_map_func)(struct cork_dllist_item *element, void *user_data);

void
cork_dllist_map(struct cork_dllist *list,
                cork_dllist_map_func func, void *user_data);


size_t
cork_dllist_size(const struct cork_dllist *list);


#define cork_dllist_add(list, element) \
    do { \
        (list)->head.prev->next = (element); \
        (element)->prev = (list)->head.prev; \
        (list)->head.prev = (element); \
        (element)->next = &(list)->head; \
    } while (0)


#define cork_dllist_remove(element) \
    do { \
        (element)->prev->next = (element)->next; \
        (element)->next->prev = (element)->prev; \
    } while (0)


#define cork_dllist_is_empty(list) \
    (cork_dllist_is_end((list), cork_dllist_start((list))))


#define cork_dllist_head(list) \
    (((list)->head.next == &(list)->head)? NULL: (list)->head.next)
#define cork_dllist_tail(list) \
    (((list)->head.prev == &(list)->head)? NULL: (list)->head.prev)

#define cork_dllist_start(list) \
    ((list)->head.next)
#define cork_dllist_end(list) \
    ((list)->head.prev)

#define cork_dllist_is_start(list, element) \
    ((element) == &(list)->head)
#define cork_dllist_is_end(list, element) \
    ((element) == &(list)->head)


#endif /* LIBCORK_DS_DLLIST_H */
