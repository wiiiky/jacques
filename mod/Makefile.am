## Process this file with automake to produce Makefile.in


AM_CPPFLAGS = \
	-DPACKAGE_LOCALE_DIR=\""$(localedir)"\" \
	-DPACKAGE_SRC_DIR=\""$(srcdir)"\" \
	-DPACKAGE_DATA_DIR=\""$(pkgdatadir)"\"

AM_CFLAGS =\
	-Wall\
	-O0 \
	-g

LIBJAC_INCLUDEDIR=/usr/include/libjac
CFLAGS = -I$(LIBJAC_INCLUDEDIR) `pkg-config --cflags python3`
LDFLAGS = `pkg-config --libs python3`

test.so: test.c
	$(CC) $^ -shared -fPIC $(CFLAGS) $(LDFLAGS) -o $@


jacques.so: jacques.c
	$(CC) $^ -shared -fPIC $(CFLAGS) $(LDFLAGS) -o jacques.so

.PHONY: test.so jacques.so


all-local: test.so jacques.so

clean-local:
	-astyle --style=google -n *.c *.h
	-rm -f *~ *.so
