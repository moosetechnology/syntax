# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.54)
AC_INIT(syntax-core, 6.0b2, Pierre.Boullier@inria.fr and Benoit.Sagot@inria.fr)
AM_INIT_AUTOMAKE

# Checks for programs.
AC_PROG_CC
AC_PROG_RANLIB
CFLAGS=

# Checks for libraries.
# FIXME: Replace `main' with a function in `-lm':
AC_CHECK_LIB(m, main)

# Checks for header files.
AC_CHECK_HEADERS(sys/time.h)

# Checks for typedefs, structures, and compiler characteristics.

# Checks for library functions.

AC_CONFIG_FILES(	Makefile 
			 	lib/Makefile
				doc/Makefile
				doc/man/Makefile
				dico/Makefile
				dico/lib/Makefile
				lfg/Makefile
				lfg/lib/Makefile
				)
AC_OUTPUT
