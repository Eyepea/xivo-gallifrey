#!/bin/sh

set -e

# CONFIGURATION
VERSION=0.9.8g
OPENSSL_DIR=openssl-$VERSION

# SAVE PATCHES
cd $OPENSSL_DIR
tar --exclude .svn -cp patches | tar -C ../ -xvp
