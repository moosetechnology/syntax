#!/bin/sh
# executes Syntax on a bunch of files and compare the result to a previously
# recorded reference (see ./tests-oracle.sh)
# Reports the files where some differences where found
# 
# OPTION:
# "-v" shows the differences for each file one after the other (hit "q"to go to next file)
# "-b" work on the big data set (test/nist files)
#      default is to work on tests/unit-tests files


exec_dir="`dirname $0`"

diff_output="/tmp/syntax-diff.txt"

SUITE="unit-tests"

# Loop through all the provided arguments
for arg in "$@"
do
    # Check if arg1 is empty and assign the first argument to it
    if [ "$arg" = "-b" ]
    then
        SUITE="nist"
    fi
done


cd "${exec_dir}/.."

for f in `ls test/${SUITE}/*.FOR`
do
  ${exec_dir}/test.sh $* $f
done
