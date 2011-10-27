/* -*- coding: utf-8 -*-
 * ----------------------------------------------------------------------
 * Copyright © 2011, RedJack, LLC.
 * All rights reserved.
 *
 * Please see the LICENSE.txt file in this distribution for license
 * details.
 * ----------------------------------------------------------------------
 */

#include "libcork/core/types.h"
#include "libcork/ds/dllist.h"


void
cork_dllist_init(struct cork_dllist *list)
{
    list->head.next = &list->head;
    list->head.prev = &list->head;
}


void
cork_dllist_map(struct cork_dllist *list,
                cork_dllist_map_func func, void *user_data)
{
    struct cork_dllist_item  *curr = cork_dllist_start(list);
    while (!cork_dllist_is_end(list, curr)) {
        /* Extract the next pointer now, just in case func frees the
         * list item. */
        struct cork_dllist_item  *next = curr->next;
        func(curr, user_data);
        curr = next;
    }
}


size_t
cork_dllist_size(const struct cork_dllist *list)
{
    size_t  size = 0;
    struct cork_dllist_item  *curr;
    for (curr = cork_dllist_end(list);
         !cork_dllist_is_start(list, curr); curr = curr->prev) {
        size++;
    }
    return size;
}
