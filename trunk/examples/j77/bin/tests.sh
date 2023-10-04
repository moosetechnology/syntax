#!/bin/sh

exec_dir="`dirname $0`"

diff_output="/tmp/syntax-diff.txt"

if `test -n "$1" -a "$1" = "-v"`
then
    DETAILS="yes"
else
    DETAILS="no"
fi    

cd "${exec_dir}/.."

for f in `ls test/nist/*.FOR`
do	
	test_file=`basename ${f}`
	expected="test-references/${test_file}.reference"
	actual="test-references/${test_file}.actual"
	bin/f77.out -json "${f}"  > "${actual}"
	if test $DETAILS = "yes"
	then
	    diff $expected $actual > $diff_output
	    if test -s $diff_output
	    then
		less $diff_output
	    else
		rm $actual
	    fi
	else
	    diff -q $expected $actual && rm $actual
	fi
done
