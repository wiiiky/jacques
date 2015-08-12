## Process this file with automake to produce Makefile.in

AM_CPPFLAGS = \
	-DPACKAGE_LOCALE_DIR=\""$(localedir)"\" \
	-DPACKAGE_SRC_DIR=\""$(srcdir)"\" \
	-DPACKAGE_DATA_DIR=\""$(pkgdatadir)"\" \
	$(JACQUES_CFLAGS)

AM_CFLAGS =\
	 -Wall\
	 -O0 \
	 -g

bin_PROGRAMS = jacques

jacques_SOURCES = \
	main.c \
	socket.c

jacques_LDFLAGS =

jacques_LDADD = $(JACQUES_LIBS)


clean-local:
	-astyle --style=google -n *.c *.h
	-rm *~
