#!/bin/sh

exec_dir="`dirname $0`"

cd "${exec_dir}/.."

for f in `ls test/nist/*.FOR`
do	
	test_file=`basename ${f}`
	expected="test-references/${test_file}.reference"
	actual="test-references/${test_file}.actual"
	bin/f77.out -json "${f}"  > "${actual}"
	diff -q $expected $actual && rm $actual
done
