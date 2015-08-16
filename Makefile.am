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

TARGET="$(SBIN_DIR)/$(PACKAGE)"
install: all
	mkdir -p $(SBIN_DIR)
	mkdir -p $(CONF_DIR)
	if [ -f $(TARGET) ] ; \
	then \
	    mv $(TARGET) $(TARGET).old; \
	fi
	cp src/$(PACKAGE) $(TARGET)
	cp conf/$(PACKAGE).conf $(CONF_DIR)
