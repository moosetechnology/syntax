#!/bin/bash

oldpwd=`pwd`

cd ${SX}
cp -p lib/libsx.a lib/libsx.std.a
rm lib/libsx.a
cd src
export CFLAGS=-ggdb
make
cd ..
cp -p lib/libsx.a lib/libsx.g.a
cp -p lib/libsx.std.a lib/libsx.a
rm lib/libsx.std.a

cd ${oldpwd}
