#!/bin/sh
# ------------------------------------------------------------------------------
# Copyright © 2020, libcork authors.
# Please see the COPYING file in this distribution for license details.
# ------------------------------------------------------------------------------

set -e

cd "$(dirname "$0")"

AUTORECONF=${AUTORECONF:-autoreconf}
ACLOCAL=${ACLOCAL:-aclocal}
AUTOCONF=${AUTOCONF:-autoconf}
AUTOHEADER=${AUTOHEADER:-autoheader}
AUTOMAKE=${AUTOMAKE:-automake}

"${AUTORECONF}" --verbose --install --force -I m4
