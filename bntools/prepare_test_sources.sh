#!/bin/sh

VER=$(cat SOURCE-VERSION)

rm -rf tmp
mkdir tmp
cd tmp
tar xzf ../tarballs/bntools_${VER}.orig.tar.gz
cd bntools/
ln -s ../../patches/ patches
quilt push -a

