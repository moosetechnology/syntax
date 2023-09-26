#!/bin/bash

oldpwd=`pwd`

cd ${sx}
cp -p lib/libsx${sxV}.a lib/libsx${sxV}.std.a
rm lib/libsx${sxV}.a
cd src
export CFLAGS=-ggdb
make
cd ..
cp -p lib/libsx${sxV}.a lib/libsx${sxV}.g.a
cp -p lib/libsx${sxV}.std.a lib/libsx${sxV}.a
rm lib/libsx${sxV}.std.a

cd ${oldpwd}