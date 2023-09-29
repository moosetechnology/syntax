#!/bin/sh

# But de ce fichier : preparer les dossiers $sx/bin et $sx/lib pour vasy :
# Si $1 est une architecture supportée, alors
#  * on deplace bin et lib 
#  * on crée bin.$ARCH et lib.$ARCH
#  * on crée les liens bin et lib qui pointent respectivement sur bin.$ARCH et sur lib.$ARCH
# Si $1 est l'argument spécial 'restore', alors on remet les dossiers
#  bin et lib dans l'état original : tel qu'il a été sauvegardé dans bin.backup et lib.backup

# Il faut prendre en compte le scénario suivant :
#   Ce script peut etre appellé depuis un 1er dossier, pour faire la compilation dans un autre
#   dossier pointé par $sx différent du dossier courant, par exemple pour les cibles 'bootstrap-*'
#   de vasymake, où on utilise un dossier temporaire
#
# Pour supporter le scénario precedent, il faut s'assurrer qu'on est bien dans le bon dossier.
# Une manière simple de s'en assurrer, c'est de changer de dossier vers "$sx"

cd "$sx"


ARCH=$1
if [ "$#" != "1" ]
then
    echo ERROR nb args is $#
    exit 1
fi


if [ "$1" = "restore" ]
then
# special case : restore in previous state
echo "Restoring directories to the previous state"
for i in bin lib
do

    if [ -L $i ]
    then 
	/bin/rm $i
	mv $i.backup $i

    else
	# $i is not a symlink
	if [ -d $i -a ! -d $i/.svn ]
	then
	    echo "=> Vasy Error: Incoherent state : dir $i exists, but dir $i/.svn does not exist"
	    exit 1
	fi
    fi

done

exit 0

fi


echo "Syntax: preparing directories for $ARCH"

cd $sx
echo "CURRENT DIR IS `pwd`"

BINDIR=bin.$ARCH
LIBDIR=lib.$ARCH

# the next for-statement creates these directories: bin.$ARCH and lib.$ARCH

for i in $BINDIR $LIBDIR
do
    mkdir -p $i
done

echo "Copying wrapper scripts to $BINDIR"

# the next if-statement tries to find the directory
#  containing the scripts
# the directory name is stored in $SCRIPTDIR
#
# The script directory can be :
#   ./bin : after a new subversion checkout or update
#   ./bin : after a successful compilation with vasymake
#   ./bin.backup : after a failed compilation with vasymake
#   ./bin.wrappers : after an installation with vasymake
#

if [ -d bin.wrappers ]
then
    SCRIPTDIR=bin.wrappers
else
    if [ -d bin.backup ]
    then
	SCRIPTDIR=bin.backup
    else
	if [ -d bin ]
	then
	    SCRIPTDIR=bin
	else
	    echo "=> Vasy Error : can not find wrappers"
	    exit 1
	fi
    fi
fi

# the next for-statement copies all the scripts to bin.$ARCH

for i in `/bin/ls $SCRIPTDIR/*|grep -v out`
do
    cp $i $BINDIR
done


# the next for-statement saves bin and lib into bin.backup and lib.backup

for i in bin lib
do
    if [ -L $i ]
    then
	/bin/rm $i
    else
	if [ -d $i ]
	then
	    if [ -d $i.backup ]
	    then
		/bin/rm -fr $i.backup
	    fi
	    
	    mv $i $i.backup
	else
	    echo "Vasy Error : $i is not a link and not a dir"
	    exit 1
	fi
    fi
done

ln -s bin.$ARCH bin
ln -s lib.$ARCH lib

/bin/ls -l bin lib



