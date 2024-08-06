#!/bin/sh

# But de ce fichier : preparer les dossiers  bin.$ARCH, lib.$ARCH, aux.$ARCH

COMMAND=`basename "$0"`

if [ $# = 1 ]
then
	DIR=""
elif [ $# = 2 ]
then
	DIR=$2
else
	echo "$COMMAND: one or two argument(s) expected"
	exit 1
fi

if [ "$DIR" != "" -a ! -d "$DIR" ]
then
	echo "$COMMAND: $DIR is not a directory"
	exit 1
fi

# -----------------------------------------------------------------------------

PREPARE() {
	mkdir -p bin.$ARCH
	mkdir -p lib.$ARCH

	if [ "$1" = "" ]
	then
		mkdir -p aux.$ARCH
	fi

	# install the wrappers in bin.$ARCH
	# sh $SX/etc/bin/sxwrappers bin.$ARCH
}

# -----------------------------------------------------------------------------

ARCH=$1
if [ "$DIR" = "" ]
then
	cd "$SX"
	echo "starting compilation for $ARCH"
	PREPARE ""
else
	cd "$DIR"
	PREPARE "$DIR"
fi

exit 0

