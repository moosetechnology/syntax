#!/bin/sh
# ``temp-links-maker'' for SYNTAX
#
# Copyright (c) 1999 Philippe Deschamp
#
#    Permission is granted to anyone to make or distribute verbatim
#    copies of this document as received, in any medium, provided
#    that the copyright notice and this permission notice are
#    preserved, thus giving the recipient permission to redistribute
#    in turn.
#
#    Permission is granted to distribute modified versions of this
#    document, or of portions of it, under the above conditions,
#    provided also that they carry prominent notices stating who last
#    changed them.

# This shell script may be used in a SYNTAX `autogenese' makefile
# to prepare the tmp directory for the constructor to build.
# The full name of the tmpdir must be the first parameter.

# Recommended use: ${SHELL} $0 ${TMP} ; : $(MAKE)

# Modified 20050302 by PhD to create src directory
# Modified 20041116 by PhD for empty directories in order not to link to *
# Created 19991126 by PhD.

case $# in
    1)
	;;
    *)
	echo "Usage: /bin/sh $0 tmpdir." >&2;
	exit 1;;
esac


test -d "$1" || mkdir "$1"
cd "$1"
exec >/dev/null 2>&1
cd "`pwd`"
mkdir ../incl ../lib ../src

FILES=
for file in ../spec/* ../src/* ../incl/* ../lib/*;do
    case "$file" in ../*/\*);;*)FILES="$FILES '$file'";;esac
done

#eval ln `if [ -h ../../incl/sxunix.h ];then echo " -s";fi` "$FILES" .
eval ln -s "$FILES" .

exit 0
