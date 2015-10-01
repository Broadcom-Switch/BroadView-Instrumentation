AM_CFLAGS  = -D_LARGEFILE64_SOURCE -I$(top_builddir)/include -I$(top_builddir)/libmetrics -I$(top_builddir)/lib 

if STATIC_BUILD
noinst_LTLIBRARIES    = libmodbst.la
libmodbst_la_SOURCES = mod_bst.c
libmodbst_la_LDFLAGS = -export-all-symbols
else
pkglib_LTLIBRARIES    = modbst.la

modbst_la_SOURCES = mod_bst.c 
modbst_la_LDFLAGS = -module -avoid-version
modbst_la_LIBADD = $(top_builddir)/libmetrics/libmetrics.la
endif

INCLUDES = @APR_INCLUDES@
