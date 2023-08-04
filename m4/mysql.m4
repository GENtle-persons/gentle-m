dnl
dnl configure.in helper macros
dnl
dnl Initial version copied from MySQL documentation (https://dev.mysql.com/doc/ndbapi/en/ndb-start-autotools.html)
dnl

AC_DEFUN([WITH_MYSQL], [
  AC_MSG_CHECKING(for mysql_config executable)

  AC_ARG_WITH(mysql, AC_HELP_STRING([--with-mysql=PATH],[path to mysql_config binary or mysql prefix dir]), [
  if test -x $withval -a -f $withval -a "no" != "$withval"
    then
      MYSQL_CONFIG=$withval
    elif test -x $withval/bin/mysql_config -a -f $withval/bin/mysql_config
    then
     MYSQL_CONFIG=$withval/bin/mysql_config
    fi
  ], [
  if test -x /usr/local/mysql/bin/mysql_config -a -f /usr/local/mysql/bin/mysql_config
    then
      MYSQL_CONFIG=/usr/local/mysql/bin/mysql_config
    elif test -x /usr/bin/mysql_config -a -f /usr/bin/mysql_config
    then
      MYSQL_CONFIG=/usr/bin/mysql_config
    fi
  ])

  if test "x$MYSQL_CONFIG" = "x" -o "x$MYSQL_CONFIG" = "xno"
  then
    AC_MSG_RESULT("MySQL support not configured.")
    #exit 3
  else
    AC_PROG_CC
    AC_PROG_CXX

    # add regular MySQL C flags
    ADDFLAGS=`$MYSQL_CONFIG --cflags`

    # add NDB API specific C flags
    IBASE=`$MYSQL_CONFIG --include`
    ADDFLAGS="$ADDFLAGS -DUSEMYSQL $IBASE/storage/ndb"
    ADDFLAGS="$ADDFLAGS -DUSEMYSQL $IBASE/storage/ndb/ndbapi"
    ADDFLAGS="$ADDFLAGS -DUSEMYSQL $IBASE/storage/ndb/mgmapi"

    CFLAGS="$CFLAGS $ADDFLAGS"
    CXXFLAGS="$CXXFLAGS $ADDFLAGS"

    LDFLAGS="$LDFLAGS "`$MYSQL_CONFIG --libs_r`" -lndbclient"
    LDFLAGS="$LDFLAGS "`$MYSQL_CONFIG --libs_r`" -lndbclient"

    AC_MSG_RESULT($MYSQL_CONFIG)
  fi
])
