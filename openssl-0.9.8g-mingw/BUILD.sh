#!/bin/sh

set -e

# CONFIGURATION
VERSION=0.9.8g
OPENSSL_DIR=openssl-$VERSION
OPENSSL_TARBALL=openssl-$VERSION.tar.gz
PERL_PATH="c:/cygwin/bin/perl"
ZLIB_PATH="/cygwin/home/Administrateur/zlib-1.2.3/"
OPENSSL_OPTIONS="no-idea no-mdc2 no-rc5 zlib enable-tlsext threads shared"
DEREF_SYMLINKS=deref_symlinks.sh

# BUILD
tar xzvf $OPENSSL_TARBALL
cp -a patches $OPENSSL_DIR/patches
cd $OPENSSL_DIR
for each in `cat patches/series`; do
	patch -p1 < patches/$each
done
../$DEREF_SYMLINKS
PERL="$PERL_PATH" ./Configure mingw -L"$ZLIB_PATH" -I"$ZLIB_PATH" $OPENSSL_OPTIONS
../$DEREF_SYMLINKS
make --debug=j PERL="$PERL_PATH"
make report PERL="$PERL_PATH"
