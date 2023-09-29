#!/bin/sh
# ``temp-dir sweeper'' for SYNTAX
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
# to clean up the tmp directory for the constructor built.
# The full name of the tmpdir must be the first parameter.

# Modified 20041116 by PhD for in order not to create *.[ch] empty files
# Created 19991126 by PhD.

case $# in
    1)
	;;
    *)
	echo "Usage: /bin/sh $0 tmpdir." >&2
	exit 1
esac

cd "$1"

find . -type l -exec rm -f '{}' \;

rm -f *.[ch] 2> /dev/null

exit 0
