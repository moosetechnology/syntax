# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.54)
AC_INIT(syntax, 6.0b7, Pierre.Boullier@inria.fr and Benoit.Sagot@inria.fr)
AM_INIT_AUTOMAKE(subdir-objects)

# List of features of Doxygene, for now on only HTML is on by default
# Note that Doxygen can generate also man pages

DX_HTML_FEATURE(ON)
DX_CHM_FEATURE(OFF)
DX_CHI_FEATURE(OFF)
DX_MAN_FEATURE(OFF)
DX_RTF_FEATURE(OFF)
DX_XML_FEATURE(OFF)
DX_PDF_FEATURE(OFF)
DX_PS_FEATURE(OFF)

DX_INPUT(incl bnf/src DAG/src csynt/src csynt_lc/src dummy_csynt/src lecl/src lfg/src make_a_dico/src paradis/src prio/src rcg/src rcg/test/src recor/src semact/src semat/src semc/src src tables/src tdef/src util xbnf/src xcsynt/src yax/src ysx/src)
DX_INCLUDE(/usr/include bnf/incl DAG/incl/ csynt/incl/ csynt_lc/incl/ lecl/incl/ incl/ incl/util/ rcg/incl/ recor/incl/ semc/incl/ tables/incl/ xbnf/incl/ xcsynt/incl/ yax/incl/ ysx/incl/)

DX_INIT_DOXYGEN([$PACKAGE_NAME], [doxygen.cfg], [dox])

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

# CFLAGS must be set AFTER AC_CHECK_LIB and AC_CHECK_HEADERS, because these macros use gcc and otherwise fail because of warnings contained in etc/make/gccoptions
CFLAGS="-D_FILE_OFFSET_BITS=64 `cat $srcdir/etc/make/gccoptions | grep 'GCC_WARNINGS=' | sed 's/GCC_WARNINGS=//' | tr -d '\012\015'`"


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
			util/Makefile
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
			semc/Makefile
			semc/lib/Makefile
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
