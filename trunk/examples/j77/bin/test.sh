#!/bin/sh
# executes Syntax on a single file and compare the result to a previously
# recorded reference (see ./tests-oracle.sh)
# 
# OPTION:
# "-v" shows the differences


exec_dir="`dirname $0`"

diff_output="/tmp/syntax-diff.txt"

DETAILS="no"
FILE=""

# Loop through all the provided arguments
for arg in "$@"
do
    # Check if arg1 is empty and assign the first argument to it
    if [ "$arg" = "-v" ]
    then
        DETAILS="yes"
    elif [ -f "$arg" ]
    then
	FILE="$arg"
    fi
done

if [ -z "$FILE" ]
then
    echo "No source file given" >$2
    exit
fi

cd "${exec_dir}/.."

test_file=`basename ${FILE}`
test_dir_long=`dirname ${FILE}`
test_dir=`basename ${test_dir_long}`
out_folder="test-references/${test_dir}"

[ ! -d "${out_folder}" ] && mkdir "${out_folder}"

expected="${out_folder}/${test_file}.reference"
actual="${out_folder}/${test_file}.actual"

bin/f77.out -json "${FILE}" | python -m json.tool  > "${actual}"

if [ $DETAILS = "yes" ]
then
    diff $expected $actual > $diff_output
    if [ -s $diff_output ]
    then
	less $diff_output
    else
	rm $actual
    fi
else
    diff -q $expected $actual && rm $actual
fi
