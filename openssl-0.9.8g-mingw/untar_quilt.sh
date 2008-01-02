#!/bin/sh

set -e

# CONFIGURATION
VERSION=0.9.8g
OPENSSL_DIR=openssl-$VERSION
OPENSSL_TARBALL=openssl-$VERSION.tar.gz

# UNTAR / QUILT
tar xzvf $OPENSSL_TARBALL
tar --exclude .svn -cp patches | tar -C $OPENSSL_DIR/ -xvp
cd $OPENSSL_DIR
quilt push -a
