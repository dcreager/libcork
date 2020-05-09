# -*- coding: utf-8 -*-
# ------------------------------------------------------------------------------
# Copyright © 2018, libcork authors
# Please see the COPYING file in this distribution for license details.
# ------------------------------------------------------------------------------

#-------------------------------------------------------------------------------
# Dist hooks

dist-hook: dist-check-git-version dist-stamps

#-------------------------------------------------------------------------------
# .commit-stamp and .version-stamp

dist-check-git-version:
	: # Verify that the version that configure.ac recorded matches the
	: # current calculated version.
	@git_ver=`$(top_srcdir)/build-aux/calculate version $(top_srcdir) $(top_srcdir)/.version-stamp`; \
	if test "x$${git_ver}" != "x$(PACKAGE_VERSION)"; then \
	    echo "ERROR: PACKAGE_VERSION and 'git describe' version do not match:"; \
	    echo "     current 'git describe' version: $${git_ver}"; \
	    echo "     current PACKAGE_VERSION:        $(PACKAGE_VERSION)"; \
	    echo "Update PACKAGE_VERSION by running $(top_srcdir)/autogen.sh."; \
	    rm -rf "$(top_srcdir)/autom4te.cache"; \
	    exit 1; \
	fi

dist-stamps:
	@: # Generate version stamps for dist tarball.
	@echo $(GIT_SHA1) > $(distdir)/.commit-stamp
	@echo $(VERSION) > $(distdir)/.version-stamp

#-------------------------------------------------------------------------------
# Preliminaries

ACLOCAL_AMFLAGS = -I m4
AM_CPPFLAGS = \
    -I$(srcdir)/include \
    -I$(builddir)/include

lib_LTLIBRARIES = libcork.la
bin_PROGRAMS = cork-hash
check_PROGRAMS =
CLEANFILES =
EXTRA_DIST =
EXTRA_PROGRAMS =
TESTS =

#-------------------------------------------------------------------------------
# Extras

EXTRA_DIST += \
    CMakeLists.txt \
    README.markdown \
    build-aux/calculate \
    cmake \
    docs \
    extras \
    share \
    include/CMakeLists.txt \
    src/CMakeLists.txt \
    src/libcork.pc.in \
    tests/CMakeLists.txt

#-------------------------------------------------------------------------------
# libcork

libcork_includedir = $(includedir)/libcork
cli_includedir = $(includedir)/libcork/cli
config_includedir = $(includedir)/libcork/config
core_includedir = $(includedir)/libcork/core
ds_includedir = $(includedir)/libcork/ds
helpers_includedir = $(includedir)/libcork/helpers
os_includedir = $(includedir)/libcork/os
threads_includedir = $(includedir)/libcork/threads

libcork_include_HEADERS = \
    include/libcork/cli.h \
    include/libcork/config.h \
    include/libcork/core.h \
    include/libcork/ds.h \
    include/libcork/os.h \
    include/libcork/threads.h

cli_include_HEADERS = \
    include/libcork/cli/commands.h

config_include_HEADERS = \
    include/libcork/config/arch.h \
    include/libcork/config/gcc.h \
    include/libcork/config/macosx.h \
    include/libcork/config/bsd.h \
    include/libcork/config/linux.h \
    include/libcork/config/config.h \
    include/libcork/config/version.h

core_include_HEADERS = \
    include/libcork/core/hash.h \
    include/libcork/core/error.h \
    include/libcork/core/allocator.h \
    include/libcork/core/u128.h \
    include/libcork/core/attributes.h \
    include/libcork/core/byte-order.h \
    include/libcork/core/callbacks.h \
    include/libcork/core/timestamp.h \
    include/libcork/core/gc.h \
    include/libcork/core/net-addresses.h \
    include/libcork/core/types.h \
    include/libcork/core/id.h \
    include/libcork/core/api.h \
    include/libcork/core/mempool.h

ds_include_HEADERS = \
    include/libcork/ds/hash-table.h \
    include/libcork/ds/array.h \
    include/libcork/ds/managed-buffer.h \
    include/libcork/ds/ring-buffer.h \
    include/libcork/ds/stream.h \
    include/libcork/ds/bitset.h \
    include/libcork/ds/buffer.h \
    include/libcork/ds/slice.h \
    include/libcork/ds/dllist.h

helpers_include_HEADERS = \
    include/libcork/helpers/posix.h \
    include/libcork/helpers/errors.h \
    include/libcork/helpers/gc.h

os_include_HEADERS = \
    include/libcork/os/files.h \
    include/libcork/os/process.h \
    include/libcork/os/subprocess.h

threads_include_HEADERS = \
    include/libcork/threads/atomics.h \
    include/libcork/threads/basics.h

libcork_la_SOURCES = \
    src/libcork/cli/commands.c \
    src/libcork/core/allocator.c \
    src/libcork/core/error.c \
    src/libcork/core/gc.c \
    src/libcork/core/hash.c \
    src/libcork/core/id.c \
    src/libcork/core/ip-address.c \
    src/libcork/core/mempool.c \
    src/libcork/core/timestamp.c \
    src/libcork/core/u128.c \
    src/libcork/core/version.c \
    src/libcork/ds/array.c \
    src/libcork/ds/bitset.c \
    src/libcork/ds/buffer.c \
    src/libcork/ds/dllist.c \
    src/libcork/ds/file-stream.c \
    src/libcork/ds/hash-table.c \
    src/libcork/ds/managed-buffer.c \
    src/libcork/ds/ring-buffer.c \
    src/libcork/ds/slice.c \
    src/libcork/ds/stream.c \
    src/libcork/posix/directory-walker.c \
    src/libcork/posix/env.c \
    src/libcork/posix/exec.c \
    src/libcork/posix/files.c \
    src/libcork/posix/process.c \
    src/libcork/posix/subprocess.c \
    src/libcork/pthreads/thread.c

pkgconfig_DATA = src/libcork.pc

libcork_la_CPPFLAGS = $(AM_CPPFLAGS) $(CPPFLAGS) -DCORK_API=CORK_EXPORT
libcork_la_CFLAGS = $(AM_CFLAGS) $(CFLAGS) -DCORK_API=CORK_EXPORT
libcork_la_LDFLAGS = $(AM_LDFLAGS) $(LDFLAGS) -version-info 17:0:1

#-----------------------------------------------------------------------
# Utility commands

cork_hash_SOURCES = src/cork-hash/cork-hash.c
cork_hash_LDADD = libcork.la

#-----------------------------------------------------------------------
# Tests

# Standalone tests

standalone_tests = \
    test-array \
    test-bitset \
    test-buffer \
    test-core \
    test-dllist \
    test-files \
    test-gc \
    test-hash-table \
    test-managed-buffer \
    test-mempool \
    test-ring-buffer \
    test-slice \
    test-subprocess \
    test-threads \
    test-u128

EXTRA_DIST += tests/create-u128-test-cases.py
EXTRA_PROGRAMS += $(standalone_tests)

if RUN_TESTS

check_PROGRAMS += $(standalone_tests)
TESTS += $(standalone_tests)

tests_LDADD_ = libcork.la $(CHECK_LIBS)
tests_LDFLAGS_ = $(LDFLAGS) -static

test_array_SOURCES = tests/test-array.c tests/helpers.h
test_array_LDADD = $(tests_LDADD_)
test_array_LDFLAGS = $(tests_LDFLAGS_)

test_bitset_SOURCES = tests/test-bitset.c tests/helpers.h
test_bitset_LDADD = $(tests_LDADD_)
test_bitset_LDFLAGS = $(tests_LDFLAGS_)

test_buffer_SOURCES = tests/test-buffer.c tests/helpers.h
test_buffer_LDADD = $(tests_LDADD_)
test_buffer_LDFLAGS = $(tests_LDFLAGS_)

test_core_SOURCES = tests/test-core.c tests/helpers.h
test_core_LDADD = $(tests_LDADD_)
test_core_LDFLAGS = $(tests_LDFLAGS_)

test_dllist_SOURCES = tests/test-dllist.c tests/helpers.h
test_dllist_LDADD = $(tests_LDADD_)
test_dllist_LDFLAGS = $(tests_LDFLAGS_)

test_files_SOURCES = tests/test-files.c tests/helpers.h
test_files_LDADD = $(tests_LDADD_)
test_files_LDFLAGS = $(tests_LDFLAGS_)

test_gc_SOURCES = tests/test-gc.c tests/helpers.h
test_gc_LDADD = $(tests_LDADD_)
test_gc_LDFLAGS = $(tests_LDFLAGS_)

test_hash_table_SOURCES = tests/test-hash-table.c tests/helpers.h
test_hash_table_LDADD = $(tests_LDADD_)
test_hash_table_LDFLAGS = $(tests_LDFLAGS_)

test_managed_buffer_SOURCES = tests/test-managed-buffer.c tests/helpers.h
test_managed_buffer_LDADD = $(tests_LDADD_)
test_managed_buffer_LDFLAGS = $(tests_LDFLAGS_)

test_mempool_SOURCES = tests/test-mempool.c tests/helpers.h
test_mempool_LDADD = $(tests_LDADD_)
test_mempool_LDFLAGS = $(tests_LDFLAGS_)

test_ring_buffer_SOURCES = tests/test-ring-buffer.c tests/helpers.h
test_ring_buffer_LDADD = $(tests_LDADD_)
test_ring_buffer_LDFLAGS = $(tests_LDFLAGS_)

test_slice_SOURCES = tests/test-slice.c tests/helpers.h
test_slice_LDADD = $(tests_LDADD_)
test_slice_LDFLAGS = $(tests_LDFLAGS_)

test_subprocess_SOURCES = tests/test-subprocess.c tests/helpers.h
test_subprocess_LDADD = $(tests_LDADD_)
test_subprocess_LDFLAGS = $(tests_LDFLAGS_)

test_threads_SOURCES = tests/test-threads.c tests/helpers.h
test_threads_LDADD = $(tests_LDADD_)
test_threads_LDFLAGS = $(tests_LDFLAGS_)

test_u128_SOURCES = tests/test-u128.c tests/helpers.h
test_u128_CPPFLAGS = -I$(builddir)/tests $(AM_CPPFLAGS)
test_u128_LDADD = $(tests_LDADD_)
test_u128_LDFLAGS = $(tests_LDFLAGS_)

u128_tests = \
    tests/u128-tests-eq.c.in \
    tests/u128-tests-ne.c.in \
    tests/u128-tests-lt.c.in \
    tests/u128-tests-le.c.in \
    tests/u128-tests-gt.c.in \
    tests/u128-tests-ge.c.in \
    tests/u128-tests-shl.c.in \
    tests/u128-tests-shr.c.in \
    tests/u128-tests-add.c.in \
    tests/u128-tests-sub.c.in

CLEANFILES += $(u128_tests)

tests/u128-tests-eq.c.in: tests/create-u128-test-cases.py
	$(AM_V_GEN) $(PYTHON) $< eq $@
tests/u128-tests-ne.c.in: tests/create-u128-test-cases.py
	$(AM_V_GEN) $(PYTHON) $< ne $@
tests/u128-tests-lt.c.in: tests/create-u128-test-cases.py
	$(AM_V_GEN) $(PYTHON) $< lt $@
tests/u128-tests-le.c.in: tests/create-u128-test-cases.py
	$(AM_V_GEN) $(PYTHON) $< le $@
tests/u128-tests-gt.c.in: tests/create-u128-test-cases.py
	$(AM_V_GEN) $(PYTHON) $< gt $@
tests/u128-tests-ge.c.in: tests/create-u128-test-cases.py
	$(AM_V_GEN) $(PYTHON) $< ge $@
tests/u128-tests-shl.c.in: tests/create-u128-test-cases.py
	$(AM_V_GEN) $(PYTHON) $< shl $@
tests/u128-tests-shr.c.in: tests/create-u128-test-cases.py
	$(AM_V_GEN) $(PYTHON) $< shr $@
tests/u128-tests-add.c.in: tests/create-u128-test-cases.py
	$(AM_V_GEN) $(PYTHON) $< add $@
tests/u128-tests-sub.c.in: tests/create-u128-test-cases.py
	$(AM_V_GEN) $(PYTHON) $< sub $@

tests/test-u128.c: $(u128_tests)

endif RUN_TESTS

# Cram tests

TEST_EXTENSIONS = .t
EXTRA_DIST += \
    tests/ccram \
    tests/cram.py \
    tests/test-input.txt

cram_tests = \
    tests/cork-hash.t \
    tests/cork-initializer.t \
    tests/cork-test/cleanup.t \
    tests/cork-test/directory-watcher.t \
    tests/cork-test/help1-c1-s1.t \
    tests/cork-test/help1-c1-s2.t \
    tests/cork-test/help1-c1.t \
    tests/cork-test/help1-c2.t \
    tests/cork-test/help1-root.t \
    tests/cork-test/help2-c1-s1.t \
    tests/cork-test/help2-c1-s2.t \
    tests/cork-test/help2-c1.t \
    tests/cork-test/help2-c2.t \
    tests/cork-test/help2-root.t \
    tests/cork-test/help3-c1-s1.t \
    tests/cork-test/help3-c1-s2.t \
    tests/cork-test/help3-c1.t \
    tests/cork-test/help3-c2.t \
    tests/cork-test/help3-root.t \
    tests/cork-test/no-command-c1.t \
    tests/cork-test/no-command-root.t \
    tests/cork-test/run-c1-s1-f.t \
    tests/cork-test/run-c1-s1-f-t.t \
    tests/cork-test/run-c1-s1.t \
    tests/cork-test/run-c1-s1-test.t \
    tests/cork-test/run-c1-s1-t.t \
    tests/cork-test/run-c1-s2-file.t \
    tests/cork-test/run-c1-s2-f.t \
    tests/cork-test/run-c1-s2.t \
    tests/cork-test/run-c2.t \
    tests/cork-test/run-find-01.t \
    tests/cork-test/run-find-all-01.t \
    tests/cork-test/run-mkdir-01.t \
    tests/cork-test/run-paths-01.t \
    tests/cork-test/run-pwd-01.t \
    tests/cork-test/run-rm-01.t \
    tests/cork-test/run-sub-01.t \
    tests/cork-test/run-sub-02.t \
    tests/cork-test/run-sub-03.t \
    tests/cork-test/run-sub-04.t \
    tests/cork-test/run-sub-05.t \
    tests/cork-test/run-sub-06.t

EXTRA_DIST += $(cram_tests)

if RUN_TESTS

TESTS += $(cram_tests)

T_LOG_COMPILER = $(srcdir)/tests/ccram
AM_T_LOG_FLAGS = \
    --python $(PYTHON) \
    --root $(srcdir) \
    --cram $(srcdir)/tests/cram.py \
    --tests

check_PROGRAMS += cork-initializer
cork_initializer_SOURCES = \
    src/cork-initializer/init1.c \
    src/cork-initializer/init2.c \
    src/cork-initializer/main.c
cork_initializer_LDADD = libcork.la

check_PROGRAMS += cork-test
cork_test_SOURCES = src/cork-test/cork-test.c
cork_test_LDADD = libcork.la

endif

# If you have test failures during distcheck, cram won't be able to print out
# the diff, since `make distcheck` causes the unpacked distribution tarball to
# be read-only.  Uncomment the following to (temporarily) make the unpacked
# tests directory writeable, so that you can see what caused the test failure.
#distcheck-hook:
#	chmod -R ug+w $(distdir)/tests

# Test harness

@VALGRIND_CHECK_RULES@

if !RUN_TESTS
check-local:
	$(error Cannot run test suite without check and Python installed!)
endif
