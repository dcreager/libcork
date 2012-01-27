.. _hash-values:

***********
Hash values
***********

.. highlight:: c

::

  #include <libcork/core.h>


The functions in this section can be used to produce fast, good hash
values.  The implementation provides by these functions can change over
time, and doesn't have to be consistent across different platforms.  The
only guarantee is that hash values will be consistest for the duration
of the current process.

.. note::

   For the curious, libcork currently uses the public-domain
   `MurmurHash3 <http://code.google.com/p/smhasher/>`_ as its hash
   implementation.

Hashing in C code
-----------------

A common pattern would be something along the lines of::

  struct my_type {
      int  a;
      long  b;
      double  c;
      size_t  name_length;
      const char  *name;
  };

  cork_hash
  my_type_hash(const struct my_type *self)
  {
      /* hash of "struct my_type" */
      cork_hash  hash = 0xd4a130d8;
      hash = cork_hash_variable(hash, self->a);
      hash = cork_hash_variable(hash, self->b);
      hash = cork_hash_variable(hash, self->c);
      hash = cork_hash_buffer(hash, self->name, self->name_length);
      return hash;
   }

In this example, the seed value (``0xd4a130d8``) is the hash of the
constant string ``"struct my_type"``.  You can produce seed values like
this using the :ref:`cork-hash <cork-hash>` script described below::

  $ cork-hash "struct my_type"
  0xd4a130d8


.. type:: uint32_t  cork_hash

.. function:: cork_hash cork_hash_buffer(cork_hash seed, const void \*src, size_t len)

   Incorporate the contents of the given binary buffer into a hash
   value.

.. function:: cork_hash cork_hash_variable(cork_hash seed, TYPE val)

   Incorporate the contents of a variable into the hash value.  *val*
   must be an lvalue visible in the current scope.


.. _cork-hash:

Hashing from the command line
-----------------------------

Several parts of libcork use hash values as identifiers; you use a
unique string to identify part of your code, and use the hash of that
string as the actual identifier value.  We provide a command-line
utility that you can use to produce these hash values:

.. code-block:: none

   cork-hash <string>

.. describe:: <string>

   The string to hash.  This should be provided as a single argument on
   the command line, so if your string contains spaces or other shell
   meta-characters, you must enclose the string in quotes.
