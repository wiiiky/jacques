## Process this file with automake to produce Makefile.in


AM_CPPFLAGS = \
	-DPACKAGE_LOCALE_DIR=\""$(localedir)"\" \
	-DPACKAGE_SRC_DIR=\""$(srcdir)"\" \
	-DPACKAGE_DATA_DIR=\""$(pkgdatadir)"\"

AM_CFLAGS =\
	-Wall\
	-O0 \
	-g

CFLAGS =`pkg-config --cflags python3 sph`
LDFLAGS = `pkg-config --libs python3 sph`

test.so: test.c
	$(CC) $^ -shared -fPIC $(CFLAGS) $(LDFLAGS) -o $@

all-local: test.so

clean-local:
	-astyle --style=google -n *.c *.h
	-rm -f *~ *.so
