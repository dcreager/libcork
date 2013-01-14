.. _dllist:

*******************
Doubly-linked lists
*******************

.. highlight:: c

::

  #include <libcork/ds.h>

This section defines a doubly-linked list data structure.  The structure
is “invasive”, since you must place an instance of the
:c:type:`cork_dllist_item` type into the type whose instances will be
stored in the list.  The list itself is represented by the
:c:type:`cork_dllist` type.

As an example, we could define the following types for storing groups,
as well as the users within each group::

  struct group {
      const char  *group_name;
      struct cork_dllist  members;
  };

  struct user {
      const char  *username;
      const char  *real_name;
      struct cork_dllist_item  list;
  };

Note that both ``cork_dllist`` and ``cork_dllist_item`` are embedded
directly into our domain-specific types.  This means that every list
operation defined in this section is guaranteed to succeed, since no
memory operations will be involved.  (The list and any items will have
already been allocated before you try to call the list function.)

Like with any embedded ``struct``, you can use the
:c:func:`cork_container_of` macro to obtain a pointer to a ``struct
user`` if you're given a pointer to a :c:type:`cork_dllist_item`.


.. type:: struct cork_dllist

   A doubly-linked list.  The list itself is represented by a sentinel
   element, representing the empty list.


.. type:: struct cork_dllist_item

   An element of a doubly-linked list.  This type will usually be
   embedded within the type whose instances will be stored in the list.

   .. member:: struct cork_dllist_item \*next
               struct cork_dllist_item \*prev

      A pointer to the next (or previous) element in the list.  If this
      element marks the end (or beginning) of the list, then *next* (or
      *prev*) will point to the list's sentinel value.


.. function:: void cork_dllist_init(struct cork_dllist \*list)

   Initializes a doubly-linked list.  The list will initially be empty.


Querying a list
---------------

.. function:: size_t cork_dllist_size(const struct cork_dllist \*list)

   Returns the number of elements in *list*.
   
   This operation runs in :math:`O(n)` time.


.. function:: bool cork_dllist_is_empty(struct cork_dllist \*list)

   Returns whether *list* is empty.

   This operation runs in :math:`O(1)` time.


Editing a list
--------------

.. function:: void cork_dllist_add(struct cork_dllist \*list, struct cork_dllist_item \*element)

   Adds *element* to the end of *list*.  You are responsible for
   allocating the list element yourself, most likely by allocating the
   ``struct`` that you've embedded :c:type:`cork_dllist_item` into.

   .. note::

      This function assumes that *element* isn't already a member of a different
      list.  You're responsible for calling :c:func:`cork_dllist_remove()` if
      this isn't the case.  (If you don't, the other list will become
      malformed.)

   This operation runs in :math:`O(1)` time.

.. function:: void cork_dllist_add_after(struct cork_dllist_item \*pred, struct cork_dllist_item \*element)
              void cork_dllist_add_before(struct cork_dllist_item \*succ, struct cork_dllist_item \*element)

   Adds *element* to the same list that *pred* or *succ* belong to.  The
   ``_after`` variant ensures that *element* appears in the list immediately
   after *pred*.  The ``_before`` variant ensures that *element* appears in the
   list immediately before *succ*.

   .. note::

      This function assumes that *element* isn't already a member of a different
      list.  You're responsible for calling :c:func:`cork_dllist_remove()` if
      this isn't the case.  (If you don't, the other list will become
      malformed.)

   This operation runs in :math:`O(1)` time.


.. function:: void cork_dllist_remove(struct cork_dllist_item \*element)

   Removes *element* from the list that it currently belongs to.  (Note
   that you don't have to pass in a pointer to that list.)

   .. note::

      You must not call this function on a list's sentinel element.

   This operation runs in :math:`O(1)` time.


Iterating through a list
------------------------

There are two strategies you can use to access all of the elements in a
doubly-linked list: *mapping* and *iterating*.  With mapping, you write
a mapping function, which will be applied to each element in the list.
(In this case, libcork controls the loop that steps through each
element.)

.. function:: void cork_dllist_map(struct cork_dllist \*list, cork_dllist_map_func func, void \*user_data)

   Apply a function to each element in *list*.  The function is allowed
   to remove the current element from the list; this will not affect our
   ability to iterate through the remainder of the list.  The function
   will be given a pointer to the :c:type:`cork_dllist_item` for each
   element; you can use :c:func:`cork_container_of()` to obtain a
   pointer to the actual element type.

.. type:: void (\*cork_dllist_map_func)(struct cork_dllist_item \*element, void \*user_data)

   A function that can be applied to each element in a doubly-linked
   list.

For instance, you can manually calculate the number of elements in a
list as follows (assuming you didn't want to use the built-in
:c:func:`cork_dllist_size()` function, of course)::

  static void
  count_elements(struct cork_dllist_item *element, void *ud)
  {
      size_t  *count = ud;
      (*count)++;
  }

  struct cork_dllist  *list = /* from somewhere */;
  size_t  count = 0;
  cork_dllist_map(list, count_elements, &count);
  /* the number of elements is now in count */


The second strategy is to iterate through the elements yourself.

.. function:: struct cork_dllist_item \*cork_dllist_head(struct cork_dllist \*list)
              struct cork_dllist_item \*cork_dllist_start(struct cork_dllist \*list)

   Returns the element at the beginning of *list*.  If *list* is empty,
   then the ``_head`` variant will return ``NULL``, while the ``_start``
   variant will return the list's sentinel element.


.. function:: struct cork_dllist_item \*cork_dllist_tail(struct cork_dllist \*list)
              struct cork_dllist_item \*cork_dllist_end(struct cork_dllist \*list)

   Returns the element at the end of *list*.  If *list* is empty, then
   the ``_tail`` variant will return ``NULL``, while the ``_end``
   variant will return the list's sentinel element.

.. function:: bool cork_dllist_is_start(struct cork_dllist \*list, struct cork_dllist_item \*element)
              bool cork_dllist_is_end(struct cork_dllist \*list, struct cork_dllist_item \*element)

   Returns whether *element* marks the start (or end) of *list*.

With these functions, manually counting the list elements looks like::

  struct cork_dllist  *list = /* from somewhere */;
  struct cork_dllist_item  *curr;
  size_t  count = 0;
  for (curr = cork_dllist_start(list); !cork_dllist_is_end(list, curr);
       curr = curr->next) {
      count++;
  }
  /* the number of elements is now in count */

You can also count the elements in reverse order::

  struct cork_dllist  *list = /* from somewhere */;
  struct cork_dllist_item  *curr;
  size_t  count = 0;
  for (curr = cork_dllist_end(list); !cork_dllist_is_start(list, curr);
       curr = curr->prev) {
      count++;
  }
  /* the number of elements is now in count */
