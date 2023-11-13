dnl Process this file with autoconf to produce a configure script.

dnl ---------------------------------------------------------------------------
dnl Name:       configure.in
dnl Purpose:    Create configure for wxAuto sample project
dnl Author:     Linus McCabe
dnl Created:    2004-09-17
dnl RCS-ID:     $Id: configure.in,v 1.5 2005/05/09 10:38:21 magnus_manske Exp $
dnl Copyright:  (c) 2004 Linus McCabe <linus@mccabe.nu>
dnl Licence:    The wxWidgets License
dnl ---------------------------------------------------------------------------

m4_include([m4/mysql.m4])

dnl generic init
AC_INIT([GENtle],[1.9.4.99.2],[https://github.com/GENtle-persons/gentle-m/issues],[https://github.com/GENtle-persons/gentle-m])
AC_CONFIG_SRCDIR([main.cpp])
dnl AM_CONFIG_HEADER(config.h:config.h.in)
AM_INIT_AUTOMAKE
dnl AC_PROG_MAKE_SET
dnl AC_ARG_PROGRAM
dnl AC_PROG_INSTALL
dnl AC_PROG_LIBTOOL

dnl check for programs we use
AC_PROG_CXX
AC_PROG_CC

dnl Finds linker and sets $LD
dnl LT_PATH_LD # needs libtool

dnl Defined in m4/mysql.m4
WITH_MYSQL()

WXCONFIG=wx-config
AC_ARG_WITH([wxconfig],
            AS_HELP_STRING([--with-wx-config],[Give path to wx-config to determine wxWidgets configuration; defaults to "wx-config".]),
            [],[])
AS_IF([test x$with_wxconfig != "x"],[if test "$with_wxconfig" != "yes" -a "$with_wxconfig" != ""; then WXCONFIG=$with_wxconfig; fi],[])

AC_MSG_CHECKING([wxWidgets version])
if wxversion=`$WXCONFIG --version`; then
    AC_MSG_RESULT([$wxversion])
else
    AC_MSG_RESULT([not found])
    AC_MSG_ERROR([wxWidgets is required. Try --with-wx-config.])
fi

WX_CPPFLAGS="`$WXCONFIG --cppflags`"
WX_CXXFLAGS="-std=c++11 `$WXCONFIG --cxxflags | sed -e 's/-fno-exceptions//'`"
WX_LIBS="`$WXCONFIG --libs`"

CPPFLAGS="$CPPFLAGS $WX_CPPFLAGS"
CXXFLAGS="$CXXFLAGS $WX_CXXFLAGS"

if [[ "x" != "x$PREFIX" ]]; then
  CPPFLAGS="$CPPFLAGS -I$PREFIX/include"
  CXXFLAGS="$CXXFLAGS -I$PREFIX/include"
  LDFLAGS="$LDFLAGS -L$PREFIX/lib"
fi

AC_SUBST(WX_LIBS)
AC_SUBST(LDFLAGS)
AC_SUBST(CPPFLAGS)
AC_SUBST(CXXFLAGS)

dnl Clarify inclusion/exclusion of ClustalW source tree
AC_ARG_WITH([clustalw],AS_HELP_STRING([--with-clustalw],[Use external binary of clustalw for sequence alignments (default=yes)]),[],[])

if test "x$with_clustalw" != "xno"; then
  AC_DEFINE([USE_EXTERNAL_CLUSTALW], [1], [Define to use the clustalw binary in your PATH for sequence alignments from within GENtle.])
fi

dnl Set debug flags
AC_ARG_ENABLE(debug,AS_HELP_STRING([--enable-debug],[Enable debug build]),
[
  debugbuild="y"
  CXXFLAGS="${CXXFLAGS} -ggdb"
])

dnl That's all, folks!
AC_CONFIG_FILES([Makefile])
AC_OUTPUT

dnl Summarized output
echo
echo "***************************************"
echo

if [[ "x$debugbuild" = "xy" ]]; then
echo "Debug               Enabled"
else
echo "Debug               Disabled"
fi

if [[ "x$with_clustalw" != "xno" ]]; then
echo "ClustalW            External binary"
else
echo "ClustalW            Self-compiled binary"
fi

if [[ "x$with_mysql" != "xno" ]]; then
echo "MySQL               Enabled"
else
echo "MySQL               Disabled"
fi

echo "CPPFLAGS=$CPPFLAGS"
echo "CXXFLAGS=$CXXFLAGS"
echo "WX_LIBS=$WX_LIBS"
echo "LDFLAGS=$LDFLAGS"
