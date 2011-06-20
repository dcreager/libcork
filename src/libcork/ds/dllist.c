/* -*- coding: utf-8 -*-
 * ----------------------------------------------------------------------
 * Copyright © 2011, RedJack, LLC.
 * All rights reserved.
 *
 * Please see the LICENSE.txt file in this distribution for license
 * details.
 * ----------------------------------------------------------------------
 */

#include <string.h>

#include "libcork/core/types.h"
#include "libcork/ds/dllist.h"


void
cork_dllist_init(cork_dllist_t *list)
{
    list->head.next = &list->head;
    list->head.prev = &list->head;
}


void
cork_dllist_map(cork_dllist_t *list,
                cork_dllist_map_func_t func, void *user_data)
{
    cork_dllist_item_t  *curr;
    for (curr = list->head.next; curr != &list->head; curr = curr->next) {
        func(curr, user_data);
    }
}


size_t
cork_dllist_size(const cork_dllist_t *list)
{
    size_t  size = 0;
    cork_dllist_item_t  *curr;
    for (curr = list->head.next; curr != &list->head; curr = curr->next) {
        size++;
    }
    return size;
}


void
cork_dllist_add(cork_dllist_t *list, cork_dllist_item_t *element)
{
    list->head.prev->next = element;
    element->prev = list->head.prev;
    list->head.prev = element;
    element->next = &list->head;
}


void
cork_dllist_remove(cork_dllist_item_t *element)
{
    element->prev->next = element->next;
    element->next->prev = element->prev;
}
