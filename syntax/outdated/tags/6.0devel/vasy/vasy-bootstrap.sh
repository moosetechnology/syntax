#!/bin/bash

# this script launches the full bootstrap process which is :
#   0) copy the $sx dir to a temporary dir
#      (this is needed because the bootstrap process modifies the source files)
#   1) compile
#   2) generate the source files with the new programs (using the makefile hypergenese)
#   3) compile these new source files
#   4) generate once again the source files with the new programs
#
# the first argument $1 is the architecture to build for

case "$1" in
	sun5 | sun64 | sol86 | sol64 | \
	sun5-gcc | sun64-gcc | sol86-gcc | sol64-gcc )
		# Solaris 
    		REMOTE_MAKE=gmake
		;;
	* )
		# GNU-like
		REMOTE_MAKE=make
		;;
esac

if [ "`uname`" = "SunOS" ]
then
    LOCAL_MAKE=gmake
else
    LOCAL_MAKE=make
fi

find_tmpdir ()
{
# this function creates a temporary directory and returns its name
# the first argument $1 is the architecture to build for
  found=0
  suffix=0
  prefix=$HOME/tmp/tmp-$1
  dirname="$prefix-$suffix"
  while [ $found -eq 0 ]
  do
    if [ -d $dirname ] || [ -f $dirname ]
    then
       suffix=`expr $suffix + 1`
       dirname="$prefix-$suffix"
       if [ $suffix -ge 10 ]
       then
          echo "error : can not find a temporary directory"
          restore_object_directory
          exit 1
       fi
    else
       found=1
    fi
  done
  echo $dirname
}


prepare_object_directory ()
{
# arg $1 is the object directory to handle : 'bin' or 'lib'
# arg $2 is the arch to build for
    cd "$sx"

# if $1 is a directory, then move it to $1.backup
    if [ -d "$1" ]
    then
	mv "$1" "$1.backup"
    fi

# if $1 is a link, then remove it
    if [ -L "$1" ]
    then
	rm "$1"
    fi

# $2 is the target architecture
# In the directory name, the dash of the architecture is turned into a dot (e.g. 'iX86-icc' becomes 'iX86.icc'). I do not know why.
    arch="`echo $2 | sed 's/-/./'`"

    ln -s $1.$arch $1

    cd -
}

restore_object_directory ()
{
    rm lib bin
    mv bin.backup bin
    mv lib.backup lib
}

check_res  ()
{
    if [ $1 -ne 0 ]
    then
	# generation failed
	echo "=> Hypergenese pass failed :("
	exit 1
    fi
    echo "=> Hypergenese pass OK"
}

hypergenese ()
{
# arg $1 is the arch to build for
# arg $2 is the remote machine to use

    prepare_object_directory bin $1

    prepare_object_directory lib $1

    EXTRA_OPTIONS=""

    case "$1" in
    sun5)
	;;
    sol86)
	EXTRA_OPTIONS="-m32"
	;;
    sun64)
	EXTRA_OPTIONS="-xarch=v9"
	;;
    sol64)
	EXTRA_OPTIONS="-m64"
	;;
    sun5-gcc | sol86-gcc)
        COMPILER=gcc
        ;;
    sun64-gcc | sol64-gcc)
        EXTRA_OPTIONS="-m64"
	COMPILER=gcc
	;;
    iX86-icc)
	COMPILER=icc
	;;
    *)
	;;
    esac

    if [ -z "$COMPILER" ]
    then
	COMPILER=cc
    fi

    res=1
    ssh $2 " \
	source /common/com/path ;\
	cd $sx                  ;\
	setenv sx $sx           ;\
	setenv sxV $sxV         ;\
	set path = ( $sx/bin \$path ) ;\
	$REMOTE_MAKE -f hypergenese nickel chrome COMPILER=$COMPILER CFLAGS=\"$EXTRA_OPTIONS $CFLAGS\" LDFLAGS=$LDFLAGS " && res=0

    restore_object_directory

    check_res $res
}

main ()
{
    if [ -z "$COMPILER" ]
    then
	COMPILER=cc
    fi

    echo "=> Boostrap for arch $1 (sx : \"$sx\")"

    arch=`echo $1 | cut -d- -f1`
    mach=`Make-mach $arch`

    if [ -n "`echo $mach | grep 'unknown architecture'`" ]
    then
	echo "=> bootstrap not supported for arch $1"
	exit 1
    fi
    echo "=> Bootstrap : will use the machine $mach"


    if [ "$1" = "x64" ]
    then
	echo "=> Bootstrap : special case x64"
	remote_sx=/home/$USER/tmp/syntax-bootstrap-x64
	# special case...
	ssh $mach "\
	    source /common/com/path           ;\
	    setenv sx $remote_sx      ;\
	    cd \$sx                            ;\
	    setenv sxV $sxV                   ;\
	    set path = ( $remote_sx/bin \$path )     ;\
	    echo \"=> Compiling for arch x86_64 (PATH= \$path)\" ;\
	    $REMOTE_MAKE -f supermake nickel chrome COMPILER=\"$COMPILER\" CFLAGS=\"$CFLAGS\" LDFLAGS=\"$LDFLAGS\"    && \
	    echo \"=> Generating source files (1st pass)\" && \
	    $REMOTE_MAKE -f hypergenese nickel chrome COMPILER=\"$COMPILER\" CFLAGS=\"$CFLAGS\" LDFLAGS=\"$LDFLAGS\"  && \
	    echo \"=> Compiling for arch x86_64 \" && \
	    $REMOTE_MAKE -f supermake nickel chrome COMPILER=\"$COMPILER\" CFLAGS=\"$CFLAGS\" LDFLAGS=\"$LDFLAGS\"    && \
	    echo \"=> Generating source files (2nd pass)\" && \
	    $REMOTE_MAKE -f hypergenese nickel chrome COMPILER=\"$COMPILER\" CFLAGS=\"$CFLAGS\" LDFLAGS=\"$LDFLAGS\"  "
	exit 0
    fi


    # step 0

    tmpdir=`find_tmpdir $1`
    echo "=> Bootstrap : copying to temp dir\"$tmpdir\" ..."
    /common/com/cpdir $sx/ $tmpdir/
    export sx="$tmpdir"

    # step 1 
    $LOCAL_MAKE -f vasy/vasymake vasy-$1 COMPILER="$COMPILER" CFLAGS="$CFLAGS" LDFLAGS="$LDFLAGS"

    # step 2

    echo "=> Generating source files (1st pass)"
    hypergenese $1 $mach COMPILER="$COMPILER" CFLAGS="$CFLAGS" LDFLAGS="$LDFLAGS"

    # step 3

    $LOCAL_MAKE -f vasy/vasymake vasy-$1 COMPILER="$COMPILER" CFLAGS="$CFLAGS" LDFLAGS="$LDFLAGS"

    # step 4

    echo "=> Generating source files (2nd pass)"
    hypergenese $1 $mach COMPILER="$COMPILER" CFLAGS="$CFLAGS" LDFLAGS="$LDFLAGS"

    # end

    echo "=> Bootstrap : removing temp dir $tmpdir ..."
    /bin/rm -fr $tmpdir

}

if [ $# -ne 1 ]
then
  echo "Usage : `basename $0` ARCH"
  exit 1
fi


main $1


