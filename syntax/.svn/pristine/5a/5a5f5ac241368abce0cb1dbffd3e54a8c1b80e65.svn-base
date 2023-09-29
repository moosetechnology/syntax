#!/bin/sh

# Ce script est appelle par le makefile trunk/vasy/vasymake
# Il a besoin que la variable d'environnement $sx pointe sur le dossier de syntax
# Il lance la procedure de verification lint sur ce dossier

if [ ! -w "$sx/" ] 
then
    echo "Error : sx directory '$sx/' is not writeable"
    exit 1
fi


LINTFLAGS="-bhux -erroff=E_ASSIGN_INT_TO_SMALL_INT,E_CAST_INT_TO_SMALL_INT,E_PTRDIFF_OVERFLOW,E_CONSTANT_CONDITION"
EXTRA_CFLAGS=",E_GLOBAL_COULD_BE_STATIC1,E_GLOBAL_COULD_BE_STATIC2 -m"
LINT32FLAGS="$LINTFLAGS$EXTRA_CFLAGS -errchk=%all,no%longptr64 -D_FILE_OFFSET_BITS=64"
LINT64FLAGS="$LINTFLAGS$EXTRA_CFLAGS -Xarch=v9 -errchk"
CFLAGS="-I$sx/incl -I$sx/etc/incl"

# 1 : la libsx

cd $sx/src

LIST=`for I in *.c ; do (grep "THIS FILE IS NOT PART OF THE SYNTAX LIBRARY" $I > /dev/null) || echo $I ; done`

 lint $CFLAGS $LINT32FLAGS $LIST 
 lint $CFLAGS $LINT64FLAGS $LIST

# 2 : les constructeurs

for CONSTRUCTEUR in DAG bnf csynt csynt_lc dico dummy_csynt lecl lfg make_a_dico paradis prio rcg recor semact semat tabc tables tdef xbnf xcsynt yax ysx
do
    MORECFLAGS="-I$sx/$CONSTRUCTEUR/incl -I$sx/bnf/incl"
    cd $sx/$CONSTRUCTEUR/src
    LIST=`for I in *.c ; do (grep "THIS FILE IS NOT PART OF THE SYNTAX LIBRARY" $I > /dev/null) || echo $I ; done`

    lint $CFLAGS $MORECFLAGS $LINT32FLAGS $LIST
    lint $CFLAGS $MORECFLAGS $LINT64FLAGS $LIST


done
