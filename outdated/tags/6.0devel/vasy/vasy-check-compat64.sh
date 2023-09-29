#!/bin/sh

# Ce script a besoin que la variable d'environnement $sx pointe sur le dossier de syntax
# Il verifie que le code source de ce dossier ne contient pas de mots cles deconseilles
# comme par exemple "register"

if [ ! -w "$sx/" ]
then
    echo "Error : sx directory '$(sx)/' is not writeable (maybe the sx variable is not set?)"
    exit 1
fi

display_header()
{
    echo "These are the uses of the forbidden word '$*' ->"
}

find_forbidden_word()
{
    display_header "$*"
    cd $sx
    find  -name "*.[ch]"| grep -vw oldies | xargs grep --color -nw "$*"
    echo "==============================================================================="
}

# 1 ) 'int'
find_forbidden_word "int"

# 2 ) //
find_forbidden_word "\/\/"

# 3 ) 'abort()'
find_forbidden_word "abort *(.*;"

# 4 ) 'errno'
find_forbidden_word "errno.*;"

# 5 ) 'register'
find_forbidden_word "register .*;"

# 6 ) 'exit()'
find_forbidden_word "exit *(.*;"

# 7 ) 'index()' and 'rindex()'
find_forbidden_word "index *(.*;"
find_forbidden_word "rindex *(.*;"

# 8 ) 'SXINT main()'
find_forbidden_word "SXINT *main *(.*;"

# 9 ) 'long long'
find_forbidden_word "long *long.*;"

# 10 ) 'unsigned long' 'unsigned long int' 'unsigned SXINT'
find_forbidden_word 'unsigned *long.*;'
find_forbidden_word 'unsigned *SXINT.*;'
