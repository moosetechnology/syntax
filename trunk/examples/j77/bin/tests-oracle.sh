#!/bin/sh
# create reference results (oracle) for the output of Syntax parser on
# various example files

exec_dir="`dirname $0`"

cd "${exec_dir}/.."


rm test-references/*

echo "Recording oracle for test/unit-tests/*.FOR"
for f in `ls test/unit-tests/*.FOR`
do
	test_file=`basename ${f}`
	actual="test-references/${test_file}.reference"
	bin/f77.out -json "${f}" | python -m json.tool > "${actual}"
done

echo "Recording oracle for test/nist/*.FOR"
for f in `ls test/nist/*.FOR`
do
	test_file=`basename ${f}`
	actual="test-references/${test_file}.reference"
	bin/f77.out -json "${f}" | python -m json.tool > "${actual}"
done
