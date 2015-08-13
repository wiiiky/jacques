dnl Process this file with autoconf to produce a configure script.
dnl Created by Anjuta application wizard.


AC_INIT(jacques, 0.01)

#AC_CONFIG_HEADERS([src/config.h])

AM_INIT_AUTOMAKE

AM_SILENT_RULES([yes])

AC_PROG_CC


PKG_CHECK_MODULES(JACQUES, [jlib-0.1 jio-0.1 jconf-0.1])


INSTALL_ROOT='$(prefix)/$(PACKAGE)'
INSTALL_SBIN='$(INSTALL_ROOT)/sbin'
INSTALL_CONF='$(INSTALL_ROOT)/conf'
AC_SUBST(INSTALL_ROOT)
AC_SUBST(INSTALL_SBIN)
AC_SUBST(INSTALL_CONF)

AC_OUTPUT([
           Makefile
           src/Makefile
           tests/Makefile
           ])
