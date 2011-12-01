/* -*- coding: utf-8 -*-
 * ----------------------------------------------------------------------
 * Copyright © 2011, RedJack, LLC.
 * All rights reserved.
 *
 * Please see the LICENSE.txt file in this distribution for license
 * details.
 * ----------------------------------------------------------------------
 */

#ifndef LIBCORK_CORE_ALLOCATOR_H
#define LIBCORK_CORE_ALLOCATOR_H

/**
 * @file
 * @brief Implementation of the @ref allocator and @ref hier_alloc
 * submodules
 */

#include <libcork/core/types.h>

/**
 * @addtogroup allocator
 *
 * <tt>#%include &lt;libcork/core/allocator.h&gt;</tt>
 *
 * The functions in this section allow you to control how libcork
 * allocates memory for its internal objects.
 *
 * The allocator is provided as a single generic function
 * (cork_alloc_func_t), which can emulate the standard malloc, realloc,
 * and free functions.  The design of this allocation interface is
 * inspired by the implementation of the Lua interpreter.
 *
 * @{
 */


struct cork_alloc;


/**
 * @brief A function that can allocate, reallocate, or free a memory
 * buffer.
 *
 * The @a ptr parameter will be the location of any existing memory
 * buffer.  The @a osize parameter will be the size of this existing
 * buffer.  If @a ptr is @c NULL, then @a osize will be 0.  The @a nsize
 * parameter will be the size of the new buffer, or 0 if the new buffer
 * should be freed.
 *
 * If @a nsize is 0, then the allocation function must return @c NULL.
 * If @a nsize is not 0, then it should return @c NULL if the allocation
 * fails.
 *
 * @param[in] alloc  A reference to the allocator object, in case you
 * need access to any additional state that you've stashed in there.
 *
 * @param[in] ptr  For reallocations and frees, a pointer to the
 * existing memory buffer.  For new allocations, @c NULL.
 *
 * @param[in] osize  For reallocations and frees, the size of the buffer
 * pointed to by @a ptr.  For new allocations, 0.
 *
 * @param[in] nsize  For new allocations and reallocations, the desired
 * size of the memory buffer.  For frees, 0.
 *
 * @returns For new allocations and reallocations, a pointer to the new
 * memory buffer, or @c NULL if the buffer cannot be (re)allocated.  For
 * frees, @c NULL.
 *
 * @since 0.2
 */

typedef void *
(*cork_alloc_func)(struct cork_alloc *alloc, void *ptr,
                   size_t osize, size_t nsize);


/**
 * @brief An object that encapsulates a custom allocation function.
 *
 * If you need to pass additional information into the allocation
 * function, you can create a subclass of this struct:
 *
 * @code
 * struct custom_allocator {
 *     struct cork_alloc  parent;
 *     // any additional state here
 * };
 * @endcode
 *
 * @since 0.2
 */

struct cork_alloc {
    /**
     * @brief An allocation function.
     *
     * See @ref cork_alloc_func for details on the function's
     * interface.
     */

    const cork_alloc_func  alloc;

    /**
     * @brief A function that can free this allocator object.
     */

    void
    (*free)(struct cork_alloc *alloc);
};

/* end of allocator group */
/**
 * @}
 */


/**
 * @brief Create a new allocator object that uses the standard @c
 * malloc, @c realloc, and @c free functions.
 *
 * @public @memberof cork_alloc
 * @since 0.2
 */

struct cork_alloc *
cork_allocator_new_malloc(void);


/**
 * @brief Create a new allocator object that uses a default debugging
 * allocation function.
 *
 * @note This function is useful for test cases, but probably shouldn't
 * be used in production code.
 *
 * @public @memberof cork_alloc
 * @since 0.2
 */

struct cork_alloc *
cork_allocator_new_debug(void);


/**
 * @brief Create a new allocator object that uses the given allocation
 * function.
 *
 * This function provides a simpler interface if you don't need to
 * include extra fields in a @c struct cork_alloc subclass.
 *
 * @param[in] alloc_func  The custom allocation function to use with
 * this context.
 *
 * @public @memberof cork_alloc
 * @since 0.2
 */

struct cork_alloc *
cork_allocator_new(cork_alloc_func alloc_func);


/**
 * @brief Finalizes and frees an allocator object.
 * @param[in] alloc  The allocator object to free.
 * @public @memberof cork_alloc
 * @since 0.2
 */

void
cork_allocator_free(struct cork_alloc *alloc);


/*-----------------------------------------------------------------------
 * Raw allocation macros
 */

/**
 * @brief Allocate a new memory region using a custom allocator.
 * @param[in] alloc  The custom allocator to use
 * @param[in] size  The requested size of the new memory region
 * @returns A pointer to the new memory region, or @c NULL if the region
 * can't be allocated.
 * @public @memberof cork_alloc
 * @since 0.2
 */

#if defined(CORK_DOCUMENTATION)
void *
cork_malloc(struct cork_alloc *alloc, size_t size);
#else
#define cork_malloc(_alloc, sz) \
    ((_alloc)->alloc((_alloc), NULL, 0, (sz)))
#endif


/**
 * @brief Reallocate a memory region using a custom allocator.
 * @param[in] alloc  The custom allocator to use
 * @param[in] ptr  The memory region to resize
 * @param[in] osize  The old size of the memory region
 * @param[in] nsize  The requested new size of the memory region
 * @returns A pointer to the resized memory region (which may or may not
 * be the same as @a ptr), or @c NULL if the region can't be resized.
 * @public @memberof cork_alloc
 * @since 0.2
 */

#if defined(CORK_DOCUMENTATION)
void *
cork_realloc(struct cork_alloc *alloc, void *ptr,
             size_t osize, size_t nsize);
#else
#define cork_realloc(_alloc, ptr, osz, nsz) \
    ((_alloc)->alloc((_alloc), (ptr), (osz), (nsz)))
#endif


/**
 * @brief Free a memory region using a custom allocator.
 * @param[in] alloc  The custom allocator to use
 * @param[in] ptr  The memory region to free
 * @param[in] osize  The old size of the memory region
 * @public @memberof cork_alloc
 * @since 0.2
 */

#if defined(CORK_DOCUMENTATION)
void
cork_free(struct cork_alloc *alloc, void *ptr, size_t osize);
#else
#define cork_free(_alloc, ptr, osz) \
    ((_alloc)->alloc((_alloc), (ptr), (osz), 0))
#endif


/**
 * @brief Allocate a new object using a custom allocator.
 * @param[in] alloc  The custom allocator to use
 * @param[in] type  The name of the object type to instantiate
 * @returns A pointer to a new, empty instance of the given type, or @c
 * NULL if the instance can't be allocated.
 * @public @memberof cork_alloc
 * @since 0.2
 */

#if defined(CORK_DOCUMENTATION)
type *cork_new(struct cork_alloc *alloc, TYPE_NAME type);
#else
#define cork_new(alloc, type) \
    ((type *) cork_malloc(alloc, sizeof(type)))
#endif


/**
 * @brief Free an object using a custom allocator.
 * @param[in] alloc  The custom allocator to use
 * @param[in] type  The name of the object type to free
 * @param[in] instance  The object instance to free
 * @public @memberof cork_alloc
 * @since 0.2
 */

#if defined(CORK_DOCUMENTATION)
void cork_delete(struct cork_alloc *alloc,
                 TYPE_NAME type, type *instance);
#else
#define cork_delete(alloc, type, instance) \
    cork_free(alloc, instance, sizeof(type))
#endif

/**
 * @brief Allocate a copy of a C string.
 *
 * You must use @ref cork_strfree to free the string.
 *
 * @param[in] alloc The custom allocator to use
 * @param[in] str The string to copy
 * @public @memberof cork_alloc
 * @since 0.2-dev
 */
const char *
cork_strdup(struct cork_alloc *alloc, const char *str);

/**
 * @brief Free a string created using @ref cork_strdup
 * @param[in] alloc The custom allocator to use
 * @param[in] str The string to free
 * @public @memberof cork_alloc
 * @since 0.2-dev
 */
void
cork_strfree(struct cork_alloc *alloc, const char *str);


#endif /* LIBCORK_CORE_ALLOCATOR_H */
