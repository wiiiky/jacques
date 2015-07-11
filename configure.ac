dnl Process this file with autoconf to produce a configure script.
dnl Created by Anjuta application wizard.


AC_INIT(jacques, 0.01)

AC_CONFIG_HEADERS([config.h])

AM_INIT_AUTOMAKE

AM_SILENT_RULES([yes])

AC_PROG_CC


dnl ***************************************************************************
dnl Internationalization
dnl ***************************************************************************
IT_PROG_INTLTOOL([0.35.0])

GETTEXT_PACKAGE=jacques
AC_SUBST(GETTEXT_PACKAGE)
AC_DEFINE_UNQUOTED(GETTEXT_PACKAGE,"$GETTEXT_PACKAGE", [GETTEXT package name])

#AM_GLIB_GNU_GETTEXT


PKG_CHECK_MODULES(JACQUES, [jlib-0.1 jio-0.1])


AC_OUTPUT([
           Makefile
           src/Makefile
           po/Makefile.in
           tests/Makefile
           ])
