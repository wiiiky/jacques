## Process this file with automake to produce Makefile.in


AM_CPPFLAGS = \
	-DPACKAGE_LOCALE_DIR=\""$(localedir)"\" \
	-DPACKAGE_SRC_DIR=\""$(srcdir)"\" \
	-DPACKAGE_DATA_DIR=\""$(pkgdatadir)"\"

AM_CFLAGS =\
	-Wall\
	-O0 \
	-g

LIBJAC_INCLUDEDIR=$(top_builddir)/src/libjac
LIBEV_INCLUDEDIR=$(top_builddir)/src/libev

test.so: test.c
	$(CC) $^ -shared -fPIC -I$(LIBJAC_INCLUDEDIR) -I$(LIBEV_INCLUDEDIR) -o $@

.PHONY: test.so


all-local: test.so

clean-local:
	-astyle --style=google -n *.c *.h
	-rm -f *~ *.so
