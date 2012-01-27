.. _stream:

*****************
Stream processing
*****************

.. highlight:: c

::

  #include <libcork/ds.h>


Using a stream consumer
-----------------------

A producer of binary data should take in a pointer to a
:c:type:`cork_stream_consumer` instance.  Any data that is produced by
the stream is then sent into the consumer instance for processing.  Once
the stream has been exhausted (for instance, by reaching the end of a
file), you signal this to the consumer.  During both of these steps, the
consumer is able to signal error conditions; for instance, a stream
consumer that parses a particular file format might return an error
condition if the stream of data is malformed.  If possible, the stream
producer can try to recover from the error condition, but more often,
the stream producer will simply pass the error back up to its caller.

.. function:: int cork_stream_consumer_data(struct cork_stream_consumer \*consumer, struct cork_slice \*slice, bool is_first_chunk, struct cork_error \*err)

   Send the next chunk of data into a stream consumer.  You only have to
   ensure that *slice* is valid for the duration of this function call;
   the stream consumer is responsible for saving a copy of the data if
   it needs to be processed later.  In particular, this means that it's
   perfectly safe for *slice* to refer to a stack-allocated memory
   region.

.. function:: int cork_stream_consumer_eof(struct cork_stream_consumer \*consumer, struct cork_error \*err)

   Notify the stream consumer that the end of the stream has been
   reached.  The stream consumer might perform some final validation and
   error detection at this point.

.. function:: void cork_stream_consumer_free(struct cork_stream_consumer \*consumer)

   Finalize and deallocate a stream consumer.

File stream producer
~~~~~~~~~~~~~~~~~~~~

As an example, the following is a full implementation of a stream
producer that reads data from a file::

  #include <stdio.h>
  #include <libcork/core.h>
  #include <libcork/core/checkers.h>
  #include <libcork/ds.h>

  #define BUFFER_SIZE  65536

  static struct cork_slice_iface  slice_iface = { NULL, NULL, NULL };

  int
  stream_read_file(struct cork_stream_consumer *consumer, FILE *fp,
                   struct cork_error *err)
  {
      char  buf[BUFFER_SIZE];
      size_t  bytes_read;
      bool  first = true;

      while ((bytes_read = fread(buf, 1, BUFFER_SIZE, fp)) > 0) {
          struct cork_slice  slice = { buf, bytes_read, &slice_iface, NULL };
          rip_check(cork_stream_consumer_data(consumer, &slice, first, err));
          first = false;
      }

      if (feof(fp)) {
          return cork_stream_consumer_eof(consumer, err);
      } else {
          /* fill in an error condition */
          return -1;
      }
  }

Note that this stream producer does not take care of opening or closing
the ``FILE`` object, nor does it take care of freeing the consumer.


Writing a new stream consumer
-----------------------------

.. type:: struct cork_stream_consumer

   An interface for consumer a stream of binary data.  The producer of
   the stream will call the :c:func:`cork_stream_consumer_data()`
   function repeatedly, once for each successive chunk of data in the
   stream.  Once the stream has been exhausted, the producer will call
   :c:func:`cork_stream_consumer_eof()` to signal the end of the stream.

   .. member:: int (\*data)(struct cork_stream_consumer \*consumer, struct cork_slice \*slice, bool is_first_chunk, struct cork_error \*err)

      Process the next chunk of data in the stream.  *slice* is only
      guaranteed to be valid for the duration of this function call.  If
      you need to access the contents of the slice later, you must save
      it somewhere yourself (usually via the :c:func:`cork_slice_copy()`
      function).

      If there is an error processing this chunk of data, you should
      return ``-1`` and fill in *err*.  This error condition will be
      passed back up to the stream producer, allowing it to abort or
      recover from the error condition, as appropriate.

   .. member:: int (\*eof)(struct cork_stream_consumer \*consumer, struct cork_error \*err)

      Handle the end of the stream.  This allows you to defer any final
      validation or error detection until all of the data has been
      processed.

      If there is an error detected at this point, you should return
      ``-1`` and fill in *err*.  This error condition will be passed
      back up to the stream producer, allowing it to abort or recover
      from the error condition, as appropriate.

   .. member:: void (\*free)(struct cork_stream_consumer \*consumer)

      Free the consumer object.

File stream consumer
~~~~~~~~~~~~~~~~~~~~

As an example, the following is a full implementation of a stream
consumer that writes data to a file::

  #include <stdio.h>
  #include <libcork/core.h>
  #include <libcork/core/checkers.h>
  #include <libcork/ds.h>

  struct file_consumer {
      /* file_consumer is a subclass of cork_stream_consumer */
      struct cork_stream_consumer  parent;
      /* the file to write the data into */
      FILE  *fp;
  };

  static int
  file_consumer_data(struct cork_stream_consumer *vself,
                     struct cork_slice *slice, bool is_first,
                     struct cork_error *err)
  {
      struct file_consumer  *self =
          cork_container_of(vself, struct file_consumer, parent);
      size_t  bytes_written =
          fwrite(slice->buf, 1, slice->size, self->fp);
      /* If there was an error writing to the file, then signal this to
       * the producer */
      if (bytes_written == slice->size) {
          return 0;
      } else {
          /* fill in an error condition */
          return -1;
      }
  }

  static int
  file_consumer_eof(struct cork_stream_consumer *vself,
                    struct cork_error *err)
  {
      /* We don't close the file, so there's nothing special to do at
       * end-of-stream. */
      return 0;
  }

  static void
  file_consumer_free(struct cork_stream_consumer *vself)
  {
      struct file_consumer  *self =
          cork_container_of(vself, struct file_consumer, parent);
      cork_delete(struct file_consumer, self);
  }

  struct cork_stream_consumer *
  file_consumer_new(FILE *fp, struct cork_error *err)
  {
      struct file_consumer  *self;
      rp_check_new(struct file_consumer, self);

      self->parent.data = file_consumer_data;
      self->parent.eof = file_consumer_eof;
      self->parent.free = file_consumer_free;
      self->fp = fp
      return &self->parent;
  }

Note that this stream consumer does not take care of opening or closing
the ``FILE`` object.
