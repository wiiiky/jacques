AM_CPPFLAGS = \
	-DPACKAGE_LOCALE_DIR=\""$(localedir)"\" \
	-DPACKAGE_SRC_DIR=\""$(srcdir)"\" \
	-DPACKAGE_DATA_DIR=\""$(pkgdatadir)"\" \
	$(JACQUES_CFLAGS)


CC=gcc

all-local: test-jac.c
	$(CC) test-jac.c -o test-jac $(JACQUES_CFLAGS) $(JACQUES_LIBS)

clean-local:
	rm -f test-jac
