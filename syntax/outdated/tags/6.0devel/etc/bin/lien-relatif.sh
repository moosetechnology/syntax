#!/bin/sh
# ``relative-link-maker'' for SYNTAX
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

# Recommended use: $0 /.../source /.../destination

# Created 19991130 by PhD.

case $# in
    2)
	;;
    *)
	echo "Usage: /bin/sh $0 source destination." >&2;
	exit 1;;
esac

if [ -h $sx/incl/sxunix.h ]
  then	case "$1" in
	    /*)	case "$2" in
		    /*) (gawk '{s=s"\n"$0};END{printf"%s%c",substr(s,2),0}'<<FinDeFichier
$1
FinDeFichier
			 gawk '{s=s"\n"$0};END{printf"%s%c",substr(s,2),0}'<<FinDeFichier
$2
FinDeFichier
			) |
			gawk -v 'RS=[\0]' -v 'FS='\' -v 'OFS='\'\\\\\'\' '
				{$1=$1; chemin[NR]=$0}
			    END	{src = split (chemin[1], S, "/"); dst = split (chemin[2], D, "/"); ic = 0;
				 for (i = 1; (i < src) && (i < dst) && S[i] == D[i]; i++) {
				    ic += length (D[i]) + 1; 
				 }
				 print "ln -s " FS substr (gensub ("[^/]+", "..", "g", substr (chemin[2]"/", ic)), 5) substr (chemin[1], ic+1) FS " " FS chemin[2] FS
				}
			    ' | sh
			;;
		    *)	ln -s "$@"
			;;
		esac
		;;
	    *)	ln -s "$@"
		;;
	esac
  else	ln "$@"
fi
