# -*- coding: utf-8 -*-
# ------------------------------------------------------------------------------
# Copyright © 2018, libcork authors
# Please see the COPYING file in this distribution for license details.
# ------------------------------------------------------------------------------

AC_INIT([libcork],
        m4_esyscmd([build-aux/calculate version . .version-stamp]),
        [info@libcork.io])
AC_CONFIG_AUX_DIR([build-aux])
AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_SRCDIR([src/libcork/core/version.c])
AM_INIT_AUTOMAKE([foreign no-dist-gzip dist-xz subdir-objects])
AM_MAINTAINER_MODE([enable])  # Allow packagers to disable if they want
AM_SILENT_RULES([yes])
LT_INIT

# Generating version-stamp files
AC_PATH_PROG(GIT, [git])
if test -z "$GIT" ; then
  AC_MSG_FAILURE([cannot find 'git'])
fi

# Versions
AC_PROG_AWK
AC_PROG_SED
BASE_VERSION=`AS_ECHO([$VERSION]) | sed -e 's/\-.*//'`
AC_SUBST(CORK_VERSION_MAJOR, [`AS_ECHO([$BASE_VERSION]) | $AWK -F. '{print $1}'`])
AC_SUBST(CORK_VERSION_MINOR, [`AS_ECHO([$BASE_VERSION]) | $AWK -F. '{print $2}'`])
AC_SUBST(CORK_VERSION_PATCH, [`AS_ECHO([$BASE_VERSION]) | $AWK -F. '{print $3}'`])
AC_SUBST(CORK_VERSION, ["$VERSION"])
AC_SUBST(CORK_GIT_SHA1, m4_esyscmd([build-aux/calculate commit . .commit-stamp]))
AC_CONFIG_FILES([include/libcork/config/version.h])
AC_PROG_CC
AC_PROG_CC_C99

# TAP support
AC_PROG_AWK

# Threads
AX_PTHREAD
LIBS="$PTHREAD_LIBS $LIBS"
CFLAGS="$CFLAGS $PTHREAD_CFLAGS"
CC="$PTHREAD_CC"

# pkg-config
PKG_INSTALLDIR
AC_CONFIG_FILES([src/libcork.pc])

# Tests
AM_PATH_PYTHON([2.3], , [:])
PKG_CHECK_MODULES(CHECK, [check], [check=yes], [check=no])
AM_CONDITIONAL(RUN_TESTS, [test "$PYTHON" != : -a $check = yes])

# Valgrind support
AX_VALGRIND_DFLT([memcheck], [on])
AX_VALGRIND_DFLT([helgrind], [off])
AX_VALGRIND_DFLT([drd], [off])
AX_VALGRIND_DFLT([sgcheck], [off])
AX_VALGRIND_CHECK()

# Turn on fatal warnings by default; you can override this by setting CPPFLAGS
# to something else when running configure.
: ${CPPFLAGS="-Wall -Werror"}

AC_OUTPUT([Makefile])

cat <<EOF

------------------ Summary ------------------
 $PACKAGE_NAME version $PACKAGE_VERSION
  Prefix.........: $prefix
  C Compiler.....: $CC $CFLAGS $CPPFLAGS
  Linker.........: $LD $LDFLAGS $LIBS
---------------------------------------------

Check the above options and compile with:
 ${MAKE-make}

EOF
