#!/bin/sh

# But de ce fichier : preparer les dossiers $SX/bin et $SX/lib pour vasy :
# Si $1 est une architecture supportee, alors
#  * on deplace bin et lib 
#  * on cree bin.$ARCH et lib.$ARCH
#  * on cree les liens bin et lib qui pointent respectivement sur bin.$ARCH et sur lib.$ARCH
# Si $1 est l'argument special 'restore', alors on remet les dossiers
#  bin et lib dans l'etat original : tel qu'il a ete sauvegarde dans bin.backup et lib.backup

# The directory containing the wrappers can be :
#   ./bin : after a new subversion checkout or update
#   ./bin : after a successful compilation
#   ./bin.backup : after a failed compilation
#   ./bin.wrappers : after an installation

COMMAND=`basename "$0"`

cd "$SX"

if [ $# != 1 ]
then
	echo "$COMMAND: one argument expected"
	exit 1
fi

if [ "$1" = "restore" ]
then
	# special case : restore in previous state
	echo "Restoring directories to the previous state"
	for D in bin lib
	do
		if [ -L $D ]
		then
			# D est un lien symbolique, probablement vers $D.$arch
			rm -f $D
		fi
		if [ ! -d $D ]
		then
			if [ -r $D ]
			then
				# l'option -e de test n'existe pas en sh POSIX
				echo "$COMMAND: $SX/$D is neither a symbolic link nor a directory"
				exit 1
			elif [ -d $D.wrappers ]
			then
				# $D a ete archive' dans $D.wrappers
				# assert $D = bin
				mv $D.wrappers $D
			elif [ -d $D.backup ]
			then
				# $D a ete archive' dans $D.backup
				mv $D.backup $D
			else
				# panique : on reconstruit $D manuellement
				case $D in
					bin )
						mkdir $D
						sh $SX/etc/bin/sxwrappers $D
						;;
					lib )	
						mkdir $D
						;;
				esac
			fi
		fi
	done
	exit 0
fi

ARCH=$1

echo "preparing directories for $ARCH"

cd $SX
echo "current directory is `pwd`"

mkdir -p bin.$ARCH
mkdir -p lib.$ARCH

# install the wrappers in bin.$ARCH
sh $SX/etc/bin/sxwrappers bin.$ARCH

# the next for-statement saves bin and lib into bin.backup and lib.backup

for D in bin lib
do
	if [ -L $D ]
	then
		# D est un lien symbolique, probablement vers $D.$arch
		/bin/rm $D
	elif [ -d $D ]
	then
		# D is a directory
		if [ -d $D.backup ]
		then
			rm -rf $D.backup
		fi
		mv $D $D.backup
	elif [ -r $D ]
	then
		# something (file, socket, etc.) named $D exists
		echo "$COMMAND: $SX/$D is neither a symbolic link nor a directory"
		exit 1
	fi
done

ln -s bin.$ARCH bin
ln -s lib.$ARCH lib

