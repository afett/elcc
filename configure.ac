AC_INIT([elcc],
	[1],
	[a.fett@gmx.de],
	[libelcc],
	[https://github.com/afett/elcc])

AC_CONFIG_SRCDIR([src/editor.cc])
AC_CONFIG_AUX_DIR([build-aux])
AM_INIT_AUTOMAKE([foreign 1.11 -Wall -Wno-portability tar-pax subdir-objects])
AC_PROG_CXX
AC_CONFIG_MACRO_DIR([m4])
LT_INIT([disable-static pic-only])

my_CFLAGS="-Wall -Werror"
AC_SUBST([my_CFLAGS])

PKG_CHECK_MODULES([TSCB], [libtscb])
PKG_CHECK_MODULES([EDIT], [libedit])

AC_CONFIG_FILES([
	Makefile
	libelcc.pc
])

AC_OUTPUT
