AM_CPPFLAGS = \
	-DPACKAGE_LOCALE_DIR=\""$(localedir)"\" \
	-DPACKAGE_SRC_DIR=\""$(srcdir)"\" \
	-DPACKAGE_DATA_DIR=\""$(pkgdatadir)"\" \
	$(JACQUES_CFLAGS)



all-local: hello.c
	gcc -shared -fPIC hello.c -o hello.so $(JACQUES_CFLAGS)

install-exec-local:
	mv hello.so /etc/jacques/modules
