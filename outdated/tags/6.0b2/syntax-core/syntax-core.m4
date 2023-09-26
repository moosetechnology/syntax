# AC_CHECK_SYNTAX_CORE
# Find some useful SYNTAX directories and libraries

AC_DEFUN([AC_CHECK_SYNTAX_CORE], [
	AC_MSG_CHECKING(for SYNTAX core)

	AC_ARG_WITH(
		syntax-core-prefix,
		AC_HELP_STRING(
			--with-syntax-core-prefix=PFX,
			syntax core prefix
		),
		syntaxcoreprefix=$withval
		if test "x$syntaxcoreprefix" != "x" && !(test -d $syntaxcoreprefix); then
			AC_MSG_ERROR("syntax core prefix is not a valid directory (\$syntaxcoreprefix=$syntaxcoreprefix)")
		fi
		,
		syntaxcoreprefix=""
	)

	AC_ARG_WITH(
		syntaxlibdir,
		AC_HELP_STRING(
			--with-syntaxlibdir=DIR,
			syntax libraries directory
		),
		syntaxlibdir=$withval,
		if test -d $syntaxprefix/usr/local/lib/syntax; then
			syntaxlibdir=$syntaxprefix/usr/local/lib/syntax
		else
			if test -d $syntaxprefix/usr/local/lib/syntax-core; then
				syntaxlibdir=$syntaxprefix/usr/local/lib/syntax-core
			else
				AC_MSG_ERROR("syntax libraries directory not found")
			fi
		fi
	)

	AC_ARG_WITH(
		syntaxlib,
		AC_HELP_STRING(
			--with-syntaxlib=DIR,
			syntax library
		),
		syntaxlib=$withval,
		if test -f $syntaxlibdir/libsx.a; then
		 	syntaxlib=$syntaxlibdir/libsx.a
		else
			AC_MSG_ERROR("syntax library libsx.a not found (\$syntaxlibdir=$syntaxlibdir)")
		fi
	)

	AC_ARG_WITH(
		syntaxincl,
		AC_HELP_STRING(
			--with-syntaxincl=DIR,
			syntax include directory
		),
		syntaxincl=$withval,
		if test -d $syntaxcoreprefix/usr/local/include/syntax; then
			syntaxincl=$syntaxcoreprefix/usr/local/include/syntax
		else
			if test -d $syntaxcoreprefix/usr/local/include/syntax-core; then
				syntaxincl=$syntaxcoreprefix/usr/local/include/syntax-core
			else
				AC_MSG_ERROR("syntax include directory not found (\$syntaxlibdir=$syntaxlibdir)")
			fi
		fi
	)

	AC_ARG_WITH(
		syntaxsrc,
		AC_HELP_STRING(
			--with-syntaxsrc=DIR,
			syntax sources directory
		),
		syntaxsrc=$withval,
		if test -d $syntaxcoreprefix/usr/local/share/syntax; then
			syntaxsrc=$syntaxcoreprefix/usr/local/share/syntax
		else
			if test -d $syntaxcoreprefix/usr/local/share/syntax-core; then
				syntaxsrc=$syntaxcoreprefix/usr/local/share/syntax-core
			else
				AC_MSG_ERROR("syntax share directory not found")
			fi
		fi
	)

	AC_ARG_WITH(
		syntaxbin,
		AC_HELP_STRING(
			--with-syntaxbin=DIR,
			syntax bin directory
		),
		syntaxbin=$withval,
		if test -f $syntaxcoreprefix/usr/local/bin/dico; then
			syntaxbin=$syntaxcoreprefix/usr/local/bin
		else
			AC_MSG_ERROR("syntax binaries not found")
		fi
	)

	SYNTAX_CPPFLAGS="-I$syntaxincl"
	SYNTAX_LIB="$syntaxlib"
	AC_SUBST(syntaxincl)
	AC_SUBST(syntaxlibdir)
	AC_SUBST(syntaxlib)
	AC_SUBST(syntaxbin)
	AC_SUBST(syntaxsrc)
	AC_SUBST(SYNTAX_CPPFLAGS)
	AC_SUBST(SYNTAX_LIB)
	AC_MSG_RESULT(yes
		\$syntaxbin = $syntaxbin
		\$syntaxlib = $syntaxlib
		\$syntaxsrc = $syntaxsrc
		\$syntaxincl = $syntaxincl)
])
