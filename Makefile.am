## Process this file with automake to produce Makefile.in
## Created by Anjuta

SUBDIRS = src po tests

dist_doc_DATA = \
	README \
	COPYING \
	AUTHORS \
	ChangeLog \
	INSTALL \
	NEWS


INTLTOOL_FILES = intltool-extract.in \
	intltool-merge.in \
	intltool-update.in

EXTRA_DIST = \
	$(INTLTOOL_FILES)

DISTCLEANFILES = intltool-extract \
	intltool-merge \
	intltool-update \
	po/.intltool-merge-cache


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
