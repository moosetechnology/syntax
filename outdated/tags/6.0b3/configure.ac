# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.54)
AC_INIT(syntax, 6.0b3, Pierre.Boullier@inria.fr and Benoit.Sagot@inria.fr)
AM_INIT_AUTOMAKE

# Checks for programs.
AC_PROG_CC
AM_PROG_CC_C_O
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
			etc/Makefile
			etc/lib/Makefile
			bnf/Makefile
			bnf/lib/Makefile
			csynt/Makefile
			csynt/lib/Makefile
			csynt_lc/Makefile
			csynt_lc/lib/Makefile
			dummy_csynt/Makefile
			dummy_csynt/lib/Makefile
			lecl/Makefile
			lecl/lib/Makefile
			make_a_dico/Makefile
			make_a_dico/lib/Makefile
			paradis/Makefile
			paradis/lib/Makefile
			prio/Makefile
			prio/lib/Makefile
			rcg/Makefile
			rcg/lib/Makefile
			recor/Makefile
			recor/lib/Makefile
			semact/Makefile
			semact/lib/Makefile
			semat/Makefile
			semat/lib/Makefile
			tabc/Makefile
			tabc/lib/Makefile
			tables/Makefile
			tables/lib/Makefile
			tdef/Makefile
			tdef/lib/Makefile
			xbnf/Makefile
			xbnf/lib/Makefile
			xcsynt/Makefile
			xcsynt/lib/Makefile
			yax/Makefile
			yax/lib/Makefile
			ysx/Makefile
			ysx/lib/Makefile
			DAG/Makefile
			DAG/lib/Makefile
			)
AC_OUTPUT
