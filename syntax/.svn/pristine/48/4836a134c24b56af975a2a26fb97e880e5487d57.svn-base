# AC_CHECK_SYNTAX
# Test for syntax directory and version, and find useful SYNTAX directories and libraries

AC_DEFUN([AC_CHECK_SYNTAX], [
	AC_MSG_CHECKING(for SYNTAX)

	AC_ARG_WITH(
		syntax-prefix,
		AC_HELP_STRING(
			--with-syntax-prefix=PFX,
			syntax prefix
		),
		syntaxprefix=$withval
		if test "x$syntaxprefix" != "x" && !(test -d $syntaxprefix); then
			AC_MSG_ERROR("syntax prefix is not a valid directory (\$syntaxprefix=$syntaxprefix)")
		fi
		,
		syntaxprefix=""
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

# 	AC_ARG_WITH(
# 		syntaxetclib,
# 		AC_HELP_STRING(
# 			--with-syntaxetclib=DIR,
# 			syntax etc-library
# 		),
# 		syntaxetclib=$withval,
# 		if test -f $syntaxlibdir/libsxetc.a; then
# 		 	syntaxlib=$syntaxlibdir/libsxetc.a
# 		else
# 			AC_MSG_ERROR("syntax 'etc' library libsxetc.a not found (\$syntaxlibdir=$syntaxlibdir)")
# 		fi
# 	)

	AC_ARG_WITH(
		syntaxincl,
		AC_HELP_STRING(
			--with-syntaxincl=DIR,
			syntax include directory
		),
		syntaxincl=$withval,
		if test -d $syntaxprefix/usr/local/include/syntax; then
			syntaxincl=$syntaxprefix/usr/local/include/syntax
		else
			if test -d $syntaxprefix/usr/local/include/syntax-core; then
				syntaxincl=$syntaxprefix/usr/local/include/syntax-core
			else
				AC_MSG_ERROR("syntax include directory not found")
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
		if test -d $syntaxprefix/usr/local/share/syntax; then
			syntaxsrc=$syntaxprefix/usr/local/share/syntax
		else
			if test -d $syntaxprefix/usr/local/share/syntax-core; then
				syntaxsrc=$syntaxprefix/usr/local/share/syntax-core
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
		if test -d $syntaxprefix/usr/local/bin; then
			syntaxbin=$syntaxprefix/usr/local/bin
		else
			AC_MSG_ERROR("bin executables not found")
		fi
	)

	SYNTAX_CPPFLAGS="-I$syntaxincl"
	SYNTAX_LIB="$syntaxlib $syntaxetclib"
	AC_SUBST(syntaxincl)
	AC_SUBST(syntaxlib)
	AC_SUBST(syntaxlibdir)
#	AC_SUBST(syntaxetclib)
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
