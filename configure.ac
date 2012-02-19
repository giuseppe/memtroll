AC_INIT(memtroll, [1.0], [gscrivano@gnu.org])

AC_CONFIG_AUX_DIR([build-aux])
AC_CONFIG_HEADERS([config.h])

AC_CONFIG_SRCDIR(src/memtroll.c)

AM_INIT_AUTOMAKE
LT_INIT([disable-static])

AC_PROG_CC
AC_PROG_RANLIB
AC_SEARCH_LIBS([dlopen], [dl])

AC_CONFIG_FILES(Makefile src/Makefile)
AC_OUTPUT