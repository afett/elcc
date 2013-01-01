ACLOCAL_AMFLAGS = -I m4 ${ACLOCAL_FLAGS}

AM_CPPFLAGS = \
	-I${top_srcdir}/elcc \
	-I${top_srcdir}/src

AM_CFLAGS = ${my_CFLAGS}

AM_LDFLAGS = \
	-Wl,--as-needed


pkginclude_HEADERS = \
	elcc/common.h \
	elcc/completion.h \
	elcc/editor.h \
	elcc/history.h \
	elcc/tscb/editor.h

lib_LTLIBRARIES = src/libelcc.la

src_libelcc_la_SOURCES = \
	src/editor_impl.h \
	src/history_impl.h \
	src/tokenizer.h \
	src/completion.cc \
	src/editor.cc \
	src/editor_impl.cc \
	src/history_impl.cc \
	src/sock_shell.cc \
	src/tokenizer.cc \
	src/tscb_editor.cc

src_libelcc_la_LIBADD = $(TSCB_LIBS) $(EDIT_LIBS)

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = libelcc.pc
