## Process this file with automake to produce Makefile.in

SUBDIRS = libev

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
	dl.c \
	libjac/socket.c

jacques_CFLAGS = -I$(srcdir)/libev -I$(srcdir)/libjac

jacques_LDFLAGS = -lm

jacques_LDADD = \
	libev/libev.a \
	-lm

libjac_aincludedir = $(includedir)/libjac

libjac_ainclude_HEADERS = \
	libev/ev.h \
	libev/ev++.h \
	libev/event.h \
	libjac/libjac.h \
	libjac/hook.h \
	libjac/mod.h \
	libjac/socket.h

clean-local:
	-astyle --style=google -n *.c *.h
	-rm -f *~
