.. _files:

*********************
Files and directories
*********************

.. highlight:: c

::

  #include <libcork/os.h>

The functions in this section let you interact with files and directories in the
local filesystem.


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
