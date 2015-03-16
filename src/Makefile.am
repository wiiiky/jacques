## Process this file with automake to produce Makefile.in

## Created by Anjuta

AM_CPPFLAGS = \
	-DPACKAGE_LOCALE_DIR=\""$(localedir)"\" \
	-DPACKAGE_SRC_DIR=\""$(srcdir)"\" \
	-DPACKAGE_DATA_DIR=\""$(pkgdatadir)"\" \
	$(JACQUES_CFLAGS)

AM_CFLAGS =\
	 -Wall\
	 -g

bin_PROGRAMS = jacques

jacques_SOURCES = \
	main.c \
	config.c \
	server.c \
	module.c \
	utils.c \
	master.c \
	net.c

jacques_LDFLAGS =

jacques_LDADD = $(JACQUES_LIBS)


clean-local:
	indent -kr -ts4 -nut *.c *.h
	rm *~
