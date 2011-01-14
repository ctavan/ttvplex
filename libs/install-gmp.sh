#!/bin/bash
GMPVER=gmp-5.0.1
GMPARCH=./$GMPVER.tar.bz2

tar xjf $GMPARCH
ln -s $GMPVER gmp

cd gmp
./configure --enable-cxx --disable-shared
make
