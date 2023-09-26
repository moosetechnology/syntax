#!/bin/sh

exec_dir="`dirname $0`"

cd "${exec_dir}/.."


rm test-references/*

for f in `ls test/nist/*.FOR`
do
	test_file=`basename ${f}`
	actual="test-references/${test_file}.reference"
	bin/f77.out -json "${f}"  > "${actual}"
done
