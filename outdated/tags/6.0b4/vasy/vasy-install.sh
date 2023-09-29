#!/bin/sh

# Usage :
#   vasy-install.sh "path_to_installation_directory"
#   the sx variable must point to the source and build directory

if [ ! -w "$sx/" ] 
then
    echo "Error : sx directory '$sx/' is not writeable"
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
    echo "Error : destination directory '$DEST/' is not writeable"
    exit 1
fi

cd $sx 

mkdir -p $DEST/etc

echo "Copying source files to $DEST ..."

cp -ar etc/incl etc/src $DEST/etc 

cp -ar doc examples incl src vasy $DEST/ 

echo "Copying object files to $DEST ..."

cp -ar bin.* lib.* $DEST/

mkdir -p $DEST/lib $DEST/bin 

echo "Making symlinks ..."

cd $DEST/lib 

for ARCH_VAR in sun5 iX86 win32 macOS mac86 sun64 x64 ia64 sun5.gcc sun64.gcc iX86.icc 
do 
    if [ -d ../lib.$ARCH_VAR ] 
    then 
	ln -s ../lib.$ARCH_VAR $ARCH_VAR 
    fi 
done

cd $DEST/bin

for ARCH_VAR in sun5 iX86 win32 macOS mac86 sun64 x64 ia64 sun5.gcc sun64.gcc iX86.icc 
do 
    if [ -d ../bin.$ARCH_VAR ] 
    then
	ln -s ../bin.$ARCH_VAR $ARCH_VAR 
    fi 
done

echo "Setting permissions ..."
 
chmod go+rX $DEST -R 