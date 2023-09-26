#!/bin/sh

# Usage :
#   vasy-install.sh "path_to_installation_directory"
#   the sx variable must point to the source and build directory

ALL_ARCH="sol86 sol64 iX86 x64 win32 mac64"
# ALL_ARCH="sun5 sun64 sol86 sol64 iX86 x64 ia64 win32 macOS mac86 mac64"

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

mkdir -p "$DEST/etc"

echo "Make clean ..."

make -f supermake clean 1>/dev/null 2>&1

$sx/vasy/vasy-prepare-directories.sh restore

echo "Copying source files to $DEST ..."

cp -pPr bin "$DEST/bin.wrappers"

cp -pPr lib "$DEST/lib.wrappers"

cp -pPr force_make Makefile.am configure.ac hypergenese s.Makefile supermake sxmakefile syntax.m4 "$DEST"

cp -pPr etc/lib etc/force_make etc/bin etc/incl etc/src etc/gccoptions etc/malloc_scribble_options.sh etc/s.Makefile etc/sxmakefile etc/valgrind "$DEST/etc"

cp -pPr LICENSE doc examples incl src vasy bnf lecl csynt tabc recor tables prio paradis semat "$DEST/"

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
    cp -pPr bin.$ARCH_VAR lib.$ARCH_VAR "$DEST/"
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

echo "Choosing default for libsx.a : DETERMINISTIC version"
for ARCH_VAR in $ALL_ARCH
do 
    cd "$DEST/lib.$ARCH_VAR"
    rm -f libsxdet.a
    ln -s libsxdet.$sxV.a libsxdet.a
done


echo "Setting permissions ..."
 
find "$DEST" -exec chmod go+rX \{\} \;

