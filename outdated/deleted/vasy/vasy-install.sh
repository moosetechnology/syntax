#!/bin/sh

# Usage :
#   vasy-install.sh "path_to_installation_directory"
#   the $SX variable must point to the source and build directory

COMMAND=`basename "$0"`

# ALL_ARCH="sun5 sun64 sol86 sol64 iX86 x64 ia64 win32 win64 macOS mac86 mac64"
ALL_ARCH="sol64 iX86 x64 win32 win64 mac64"

if [ ! -w "$SX/" ] 
then
	echo "$COMMAND: SYNTAX directory '$SX/' is not writeable"
	exit 1
fi

if [ $# -ne 1 ]
then
	echo "Usage : `basename $0` INSTALL_DIRECTORY"
	exit 1
fi

DEST="$1"

if [ ! -d "$DEST" ]
then
	mkdir "$DEST" || exit 1
fi

if [ ! -w "$DEST" ]
then
	echo "$COMMAND: destination directory '$DEST/' is not writeable"
	exit 1
fi

cd $SX 

mkdir -p "$DEST/etc"

echo "Make clean ..."

make -f etc/make/supermake clean 1>/dev/null 2>&1

$SX/etc/bin/vasy-prepare-directories.sh restore

echo "Copying source files to $DEST ..."

cp -pPr bin "$DEST/bin.wrappers"

cp -pPr bnf csynt doc etc examples incl lecl paradis prio recor semact semat src tabc tables tdef "$DEST/"

find "$DEST" -name "*~" | xargs /bin/rm 2>/dev/null
find "$DEST" -name ".svn" | xargs /bin/rm -fr 2>/dev/null
find "$DEST" -name "tmp" | xargs /bin/rm -fr 2>/dev/null
find "$DEST" -name "*.ln" | xargs /bin/rm -fr 2>/dev/null
find "$DEST" -name "log" | xargs /bin/rm -fr 2>/dev/null

echo "Copying object files to $DEST ..."

for ARCH_VAR in $ALL_ARCH
do 
	rm -rf "$DEST/bin.$ARCH_VAR"
	rm -rf "$DEST/lib.$ARCH_VAR"
	if [ -d bin.$ARCH_VAR -o -d lib.$ARCH_VAR ]
	then
		cp -pPr bin.$ARCH_VAR lib.$ARCH_VAR "$DEST/"
	fi
done

mkdir -p "$DEST/lib" "$DEST/bin"

echo "Making symlinks ..."

cd "$DEST/lib"

for ARCH_VAR in $ALL_ARCH
do 
	if [ -d ../lib.$ARCH_VAR ] 
	then
		rm -f $ARCH_VAR
		ln -s ../lib.$ARCH_VAR $ARCH_VAR 
	fi 
done

cd "$DEST/bin"

for ARCH_VAR in $ALL_ARCH
do 
	if [ -d ../bin.$ARCH_VAR ] 
	then
		rm -f $ARCH_VAR
		ln -s ../bin.$ARCH_VAR $ARCH_VAR 
	fi 
done

echo "Setting permissions ..."
 
find "$DEST" -exec chmod go+rX \{\} \;

