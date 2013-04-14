#!/bin/sh

srcdir=`dirname $0`
test -z "$srcdir" && srcdir=.

PKG_NAME="mousetweaks"

(test -f $srcdir/src/mt-main.c) || {
    echo -n "**Error**: Directory "\`$srcdir\'" does not look like the"
    echo " top-level $PKG_NAME directory."
    exit 1
}

# create macro directory
if test ! -d $srcdir/m4; then
    mkdir $srcdir/m4
fi

which gnome-autogen.sh || {
    echo "You need to install gnome-common from the GNOME Git (or from"
    echo " your OS vendor's package manager)."
    exit 1
}

USE_GNOME2_MACROS=1 USE_COMMON_DOC_BUILD=yes . gnome-autogen.sh
