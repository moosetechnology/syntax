# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.54)
AC_INIT(syntax, 6.0b6, Pierre.Boullier@inria.fr and Benoit.Sagot@inria.fr)
AM_INIT_AUTOMAKE

# Checks for programs.
AC_PROG_CC
AM_PROG_CC_C_O
AC_PROG_RANLIB

# Checks for libraries.
AC_CHECK_LIB(m, log)

# Checks for header files.
AC_CHECK_HEADERS([sys/time.h])

# Checks for programs.
AC_CHECK_PROG([GREP_FOUND], [grep], [yes], [no])
if test $GREP_FOUND = no; then
AC_MSG_ERROR([Please install grep before compiling and using SYNTAX])
fi
AC_CHECK_PROG([CAT_FOUND], [cat], [yes], [no])
if test $CAT_FOUND = no; then
AC_MSG_ERROR([Please install cat before compiling and using SYNTAX])
fi
AC_CHECK_PROG([SED_FOUND], [sed], [yes], [no])
if test $SED_FOUND = no; then
AC_MSG_ERROR([Please install sed before compiling and using SYNTAX])
fi
AC_CHECK_PROG([TR_FOUND], [tr], [yes], [no])
if test $TR_FOUND = no; then
AC_MSG_ERROR([Please install tr before compiling and using SYNTAX])
fi
AC_CHECK_PROG([PERL_FOUND], [perl], [yes], [no])
if test $PERL_FOUND = no; then
AC_MSG_WARN([****************************************************]);
AC_MSG_WARN([****** Some of SYNTAX's features require perl ******]);
AC_MSG_WARN([****************************************************])
fi

# CFLAGS must be set AFTER AC_CHECK_LIB and AC_CHECK_HEADERS, because these macros use gcc and otherwise fail because of warnings contained in etc/gccoptions
CFLAGS="-D_FILE_OFFSET_BITS=64 `cat $srcdir/etc/gccoptions | grep 'GCC_WARNINGS=' | sed 's/GCC_WARNINGS=//' | tr -d '\012\015'`"


#do we need the debug library?
AC_MSG_CHECKING(if we shall compile the debug SYNTAX library)
AC_ARG_ENABLE(	[debug],
				AC_HELP_STRING(	[--enable-debug],  [Enables the compilation of the debug SYNTAX library ]),
				[ compilelibsxdebug=yes ],
				[ compilelibsxdebug=no ])
AM_CONDITIONAL([LIBSXDEBUG], [test x$compilelibsxdebug = xyes])
if test x$compilelibsxdebug = xyes; then
AC_MSG_RESULT(yes)
else
AC_MSG_RESULT(no)
fi

#do we need the deterministic (sub-)library?
AC_MSG_CHECKING(if we shall compile the deterministic SYNTAX sub-library)
AC_ARG_ENABLE(	[det],
				AC_HELP_STRING(	[--enable-det],  [Enables the compilation of the deterministic SYNTAX sub-library ]),
				[ compilelibsxdet=yes ],
				[ compilelibsxdet=no ])
AM_CONDITIONAL([LIBSXDET], [test x$compilelibsxdet = xyes])
if test x$compilelibsxdet = xyes; then
AC_MSG_RESULT(yes)
else
AC_MSG_RESULT(no)
fi

abs_top_builddir=`pwd`
AC_SUBST(abs_top_builddir)

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
