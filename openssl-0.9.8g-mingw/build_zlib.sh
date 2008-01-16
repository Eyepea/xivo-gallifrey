#!/bin/sh

set -e

tar xzvf zlib-1.2.3.tar.gz
cd zlib-1.2.3
make CFLAGS="-D_REENTRANT -DPIC -O3 -W -Wall" CC=gcc
