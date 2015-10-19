## Process this file with automake to produce Makefile.in

SUBDIRS = libev

AM_CPPFLAGS = \
	-DPACKAGE_LOCALE_DIR=\""$(localedir)"\" \
	-DPACKAGE_SRC_DIR=\""$(srcdir)"\" \
	-DPACKAGE_DATA_DIR=\""$(pkgdatadir)"\" \
	-DCONFIG_DIR=\""$(CONF_DIR)"\" \
	-DLOG_DIR=\""$(LOGS_DIR)"\" \
	-DMOD_DIR=\""$(MODS_DIR)"\" \
	-I$(srcdir)/libev

AM_CFLAGS =\
	 -Wall\
	 -O0 \
	 -g

bin_PROGRAMS = jacques


jacques_SOURCES = \
	main.c \
	socket.c \
	client.c \
	server.c \
	signal.c \
	list.c

jacques_LDFLAGS = -lm

jacques_LDADD = libev/libev.a


clean-local:
	-astyle --style=google -n *.c *.h
	-rm -f *~
