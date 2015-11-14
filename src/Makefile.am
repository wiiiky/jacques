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
	signals.c \
	server.c \
	client.c

jacques_CFLAGS = $(SPH_CFLAGS)

jacques_LDFLAGS = $(SPH_LDFLAGS)

jacques_LDADD = $(SPH_LIBS)

clean-local:
	-astyle --style=google -n *.c *.h
	-rm -f *~
