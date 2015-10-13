## Process this file with automake to produce Makefile.in

SUBDIRS = libev

AM_CPPFLAGS = \
	-DPACKAGE_LOCALE_DIR=\""$(localedir)"\" \
	-DPACKAGE_SRC_DIR=\""$(srcdir)"\" \
	-DPACKAGE_DATA_DIR=\""$(pkgdatadir)"\" \
	-DCONFIG_DIR=\""$(CONF_DIR)"\" \
	-DLOG_DIR=\""$(LOGS_DIR)"\" \
	-DMOD_DIR=\""$(MODS_DIR)"\" \
	$(JACQUES_CFLAGS) \
	-I$(srcdir)/libev

AM_CFLAGS =\
	 -Wall\
	 -O0 \
	 -g \
	 -pg

bin_PROGRAMS = jacques


jacques_SOURCES = \
	main.c \
	socket.c \
	config.c \
	server.c \
	utils.c \
	master.c

jacques_LDFLAGS = -pg

jacques_LDADD = $(JACQUES_LIBS) libev/libev.a


clean-local:
	-astyle --style=google -n *.c *.h
	-rm -f *~
