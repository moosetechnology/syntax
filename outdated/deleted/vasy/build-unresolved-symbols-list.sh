#!/bin/sh

# Ce script construit la liste des symboles non
# resolus (qui ne sont pas des symboles systemes) pour n'importe quel fichier de
# type "archive ar" (extension .a). Ce script a pour vocation d'etre compatible
# avec l'outil nm pour les architectures Solaris et Linux. Il prend en parametre
# une liste de fichiers "archive ar". Il s'utilise de la maniere suivante si on
# compile syntax avec les autotools :
# 
# cd trunk
# ./vasy/build-unresolved-symbols-list.sh lib/libsx6.0b5.a lib/libsxdet.6.0b5.a

sx=`dirname $0`/..
ARCH=`$sx/vasy/arch`

if [ "`uname`" = "SunOS" ]
then
    UNDEF_STRING=UNDEF
    CUT_STRING='cut -d\| -f8'
else
    UNDEF_STRING=U
    CUT_STRING="cut -b 12-"
fi

REJECT()
{
  grep -vw "$1" | grep -vw "_$1"
}

build_unresolved_symbols_list()
{
    # arg 1 is "path to the libsx.a file"
    echo "====================================================="
    echo "Working on file $1"

    # this first version works for nm solaris-style
    # it must be ported to work with nm gnu-style 

    # it also assumes gnu grep will be used for efficient regular expressions
    
    TMPFILE_NM=`mktemp`
    TMPFILE_SYMBOLES=`mktemp`

#    echo "TMP FILES ARE $TMPFILE_NM $TMPFILE_SYMBOLES"
    /bin/rm -fr $TMPFILE_NM $TMPFILE_SYMBOLES

    nm "$1" > $TMPFILE_NM


    # Now, we are building the following list :
    #  The list of all the symbols which appear at least once with the status UNDEF
    # 
    #  Each of these symbols is stored in $TMPFILE_NM, along with the number of times
    #  it appears (elsewhere) without the status UNDEF

    for i in `cat $TMPFILE_NM|grep -w $UNDEF_STRING| eval $CUT_STRING `
	do echo -n "$i   "
	grep -w $i $TMPFILE_NM | grep -vw $UNDEF_STRING | wc -l
    done > $TMPFILE_SYMBOLES


    # We can now display the symbols that match the 2 following criteria :
    #  - is followed by the value 0
    #  - can not be found in a system library

    grep -w 0 $TMPFILE_SYMBOLES|grep -v "^__"| REJECT abort| REJECT alarm| REJECT atof| REJECT atoi| REJECT calloc| REJECT close| REJECT errno| REJECT exit| REJECT fclose| REJECT fdopen| REJECT fflush| REJECT stret4| REJECT fileno| REJECT fopen| REJECT fprintf| REJECT fputc| REJECT fputs| REJECT fread| REJECT fseek| REJECT free| REJECT fstat| REJECT ftell| REJECT fwrite| REJECT isatty| REJECT labs| REJECT log10| REJECT longjmp| REJECT malloc| REJECT memcmp| REJECT memcpy| REJECT memset| REJECT mkstemp| REJECT open| REJECT printf| REJECT puts| REJECT qsort| REJECT read| REJECT realloc| REJECT rewind| REJECT setbuf| REJECT setitimer| REJECT setjmp| REJECT signal| REJECT sprintf| REJECT strcat| REJECT strchr| REJECT strcmp| REJECT strcpy| REJECT strcspn| REJECT strerror| REJECT strlen| REJECT strncmp| REJECT strncpy| REJECT strrchr| REJECT strstr| REJECT tmpfile| REJECT write| REJECT getrusage| REJECT strdup| REJECT strtol| REJECT ctime|grep -wv time|REJECT atol|REJECT putchar|REJECT stderr|REJECT stdout|REJECT stdin|REJECT _IO_putc|REJECT _IO_getc|REJECT getchar | REJECT vfprintf | REJECT vsnprintf |REJECT _setjmp | REJECT dyld_stub_binding_helper | REJECT getc | REJECT putc | REJECT time\
	\
	|sort -u|cat -n 

    /bin/rm -fr $TMPFILE_NM $TMPFILE_SYMBOLES


}


for i in $*
do
    build_unresolved_symbols_list "$i"
done
