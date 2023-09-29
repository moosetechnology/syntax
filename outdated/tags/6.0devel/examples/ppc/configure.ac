# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.54)
AC_INIT(ppc, 6.0b5, Philippe.Deschamp@inria.fr)
AM_INIT_AUTOMAKE

# Checks for programs.
AC_PROG_CC
AM_PROG_CC_C_O
AC_CHECK_SYNTAX

# Checks for libraries.
# FIXME: Replace `main' with a function in `-lm':
AC_CHECK_LIB(m, main)

# Checks for header files.
AC_CHECK_HEADERS(sys/time.h)

# CFLAGS must be set AFTER AC_CHECK_LIB and AC_CHECK_HEADERS, because these macros use gcc and otherwise fail because of warnings contained in etc/gccoptions
CFLAGS="`cat ../../etc/gccoptions | grep 'GCC_WARNINGS=' | sed 's/GCC_WARNINGS=//' | tr -d '\012\015'`"

AC_CONFIG_FILES(	Makefile
			lib/Makefile
			)
AC_OUTPUT
