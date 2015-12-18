# configure.ac for lazytree
AC_INIT([lazytree], [0.1])
AM_INIT_AUTOMAKE([-Wall -Werror foreign])

AC_PROG_CC

PKG_CHECK_MODULES(TREEVIEW, gtk+-3.0)

CFLAGS+=" -g -O0"

AC_CONFIG_FILES([
  Makefile
])

AC_OUTPUT
