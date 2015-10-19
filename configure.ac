dnl Process this file with autoconf to produce a configure script.
dnl Created by Anjuta application wizard.


AC_INIT(jacques, 0.01)
AC_CONFIG_AUX_DIR([.])

#AC_CONFIG_HEADERS([src/config.h])

AM_INIT_AUTOMAKE

AM_SILENT_RULES([yes])

AC_PROG_CC


#PKG_CHECK_MODULES(JACQUES, [jlib-0.1 jio-0.1 jconf-0.1 jmod-0.1])


INSTALL_ROOT='$(prefix)/$(PACKAGE)'
SBIN_DIR='$(INSTALL_ROOT)/sbin'
CONF_DIR='$(INSTALL_ROOT)/conf'
LOGS_DIR='$(INSTALL_ROOT)/logs'
MODS_DIR='$(INSTALL_ROOT)/mods'
AC_SUBST(INSTALL_ROOT)
AC_SUBST(SBIN_DIR)
AC_SUBST(CONF_DIR)
AC_SUBST(LOGS_DIR)
AC_SUBST(MODS_DIR)

AC_CONFIG_SUBDIRS([src/libev])

AC_OUTPUT([
           Makefile
           src/Makefile
           ])
