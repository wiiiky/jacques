## Process this file with automake to produce Makefile.in
## Created by Anjuta

SUBDIRS = src tests

dist_doc_DATA = \
	README \
	COPYING \
	AUTHORS \
	ChangeLog \
	INSTALL \
	NEWS

uninstall:
	-rm -r $(INSTALL_ROOT)

TARGET="$(INSTALL_SBIN)/$(PACKAGE)"
install: all
	mkdir -p $(INSTALL_SBIN)
	mkdir -p $(INSTALL_CONF)
	if [ -f $(TARGET) ] ; \
	then \
	    mv $(TARGET) $(TARGET).old; \
	fi
	mv src/$(PACKAGE) $(TARGET)
