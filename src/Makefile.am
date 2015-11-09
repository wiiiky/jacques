## Process this file with automake to produce Makefile.in

AUTOMAKE_OPTIONS = subdir-objects

AM_CPPFLAGS = \
	-DPACKAGE_LOCALE_DIR=\""$(localedir)"\" \
	-DPACKAGE_SRC_DIR=\""$(srcdir)"\" \
	-DPACKAGE_DATA_DIR=\""$(pkgdatadir)"\"

AM_CFLAGS =\
	 -Wall\
	 -O0 \
	 -g

bin_PROGRAMS = jacques


jacques_SOURCES = \
	main.c \
	client.c \
	server.c \
	signal.c \
	list.c \
	mod.c

jacques_CFLAGS = -I/usr/include/libjac

jacques_LDFLAGS = -ljac

jacques_LDADD = \
	-lm \
	-ldl

clean-local:
	-astyle --style=google -n *.c *.h
	-rm -f *~
