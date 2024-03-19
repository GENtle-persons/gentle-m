#!/bin/sh

if uname | grep -q ^Darwin; then
   glibtoolize --automake --force --copy
else
   libtoolize --automake --force --copy
fi
autoreconf -i
