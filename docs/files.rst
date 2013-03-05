.. _files:

*********************
Files and directories
*********************

.. highlight:: c

::

  #include <libcork/os.h>

The functions in this section let you interact with files and directories in the
local filesystem.


Paths
=====

We provide several functions for constructing and handling paths into the local
filesystem.

.. type:: struct cork_path

   Represents a path in the local filesystem.  The path can be relative or
   absolute.  The paths don't have to refer to existing files or directories.

.. function:: struct cork_path \*cork_path_new(const char \*path)
              struct cork_path \*cork_path_clone(const struct cork_path \*other)

   Construct a new path object from the given path string, or as a copy of
   another path object.

.. function:: void cork_path_free(struct cork_path \*path)

   Free a path object.

.. function:: const char \*cork_path_get(const struct cork_path \*path)

   Return the string content of a path.  This is not normalized in any way.  The
   result is guaranteed to be non-``NULL``, but may refer to an empty string.
   The return value belongs to the path object; you must not modify the contents
   of the string, nor should you try to free the underlying memory.

.. function:: struct cork_path \*cork_path_absolute(const struct cork_path \*other)
              int cork_path_make_absolute(struct cork_path \path)

   Convert a relative path into an absolute path.  The first variant constructs
   a new path object to hold the result; the second variant overwritesthe
   contents of *path*.

   If there is a problem obtaining the current working directory, these
   functions will return an error condition.

.. function:: struct cork_path \*cork_path_join(const struct cork_path \*path, const char \*more)
              struct cork_path \*cork_path_join_path(const struct cork_path \*path, const struct cork_path \*more)
              void \*cork_path_append(struct cork_path \path, const char \*more)
              void \*cork_path_append_path(struct cork_path \*path, const struct cork_path \*more)

   Concatenate two paths together.  The ``join`` variants create a new path
   object containing the concatenated results.  The ``append`` variants
   overwrite the contents of *path* with the concatenated results.


.. function:: struct cork_path \*cork_path_basename(const struct cork_path \*path)
              void \*cork_path_set_basename(struct cork_path \*path)

   Extract the base name of *path*.  This is the portion after the final
   trailing slash.  The first variant constructs a new path object to hold the
   result; the second variant overwritesthe contents of *path*.

   .. note::

      These functions return a different result than the standard
      ``basename(3)`` function.  We consider a trailing slash to be significant,
      whereas ``basename(3)`` does not::

          basename("a/b/c/") == "c"
          cork_path_basename("a/b/c/") == ""

.. function:: struct cork_path \*cork_path_dirname(const struct cork_path \*path)
              void \*cork_path_set_dirname(struct cork_path \*path)

   Extract the directory name of *path*.  This is the portion before the final
   trailing slash.  The first variant constructs a new path object to hold the
   result; the second variant overwritesthe contents of *path*.

   .. note::

      These functions return a different result than the standard ``dirname(3)``
      function.  We consider a trailing slash to be significant, whereas
      ``dirname(3)`` does not::

          dirname("a/b/c/") == "a/b"
          cork_path_dirname("a/b/c/") == "a/b/c"


Directory walking
=================

.. function:: int cork_walk_directory(const char \*path, struct cork_dir_walker \*walker)

   Walk through the contents of a directory.  *path* can be an absolute or
   relative path.  If it's relative, it will be interpreted relative to the
   current directory.  If *path* doesn't exist, or there are any problems
   reading the contents of the directory, we'll set an error condition and
   return ``-1``.

   To process the contents of the directory, you must provide a *walker* object,
   which contains several callback methods that we will call when files and
   subdirectories of *path* are encountered.  Each method should return ``0`` on
   success.  Unless otherwise noted, if we receive any other return result, we
   will abort the directory walk, and return that same result from the
   :c:func:`cork_walk_directory` call itself.

   In all of the following methods, *base_name* will be the base name of the
   entry within its immediate subdirectory.  *rel_path* will be the relative
   path of the entry within the *path* that you originally asked to walk
   through.  *full_path* will the full path to the entry, including *path*
   itself.

   .. type:: struct cork_dir_walker

      .. member:: int (\*file)(struct cork_dir_walker \*walker, const char \*full_path, const char \*rel_path, const char \*base_name)

         Called when a regular file is encountered.

      .. member:: int (\*enter_directory)(struct cork_dir_walker \*walker, const char \*full_path, const char \*rel_path, const char \*base_name)

         Called when a subdirectory of *path* of encountered.  If you don't want
         to recurse into this directory, return :c:data:`CORK_SKIP_DIRECTORY`.

         .. macro:: CORK_SKIP_DIRECTORY

      .. member:: int (\*leave_directory)(struct cork_dir_walker \*walker, const char \*full_path, const char \*rel_path, const char \*base_name)

         Called when a subdirectory has been fully processed.
