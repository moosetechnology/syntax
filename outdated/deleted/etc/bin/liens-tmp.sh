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

# Modified 20050302 by PhD to create src directory
# Modified 20041116 by PhD for empty directories in order not to link to *
# Created 19991126 by PhD.

case $# in
    1)
	;;
    *)
	echo "Usage: /bin/sh $0 tmpdir." >&2
	exit 1
esac

mkdir -p "$1"

cd "$1"

for file in `ls ../boot/* 2> /dev/null`
do
   cp "$file" .
done

# for dir in incl spec src
# do
#    if [ -d ../$dir ]
#    then
#       for file in `ls ../$dir/* 2> /dev/null`
#       do
#          if [ ! -f `basename "$file"` ]
#          then
#             # $file n'est pas un fichier genere (*_t.c, *_td.h, etc.)
#             ln -s "$file" .
#          fi
#       done
#    fi
# done

# Hubert (sept 2023) : nouveau code pour traiter le cas ou les repertoires incl/
# src/ et spec/ ont ete supprimes dans les repertoires des processeurs de SYNTAX

for suffix in c h at bnf lecl recor semc tdef
do
	for file in `ls ../*.$suffix 2> /dev/null`
	do
		ln -s "$file" .
	done
done

exit 0
