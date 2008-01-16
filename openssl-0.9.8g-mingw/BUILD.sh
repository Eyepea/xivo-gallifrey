#!/bin/sh

set -e

# CONFIGURATION
VERSION=0.9.8g
OPENSSL_DIR=openssl-$VERSION
OPENSSL_TARBALL=openssl-$VERSION.tar.gz
PERL_PATH="c:/cygwin/bin/perl"
if [ ! -n "$ZLIB_PATH" ] ; then
	ZLIB_PATH="/cygwin/home/winbuild/zlib-1.2.3"
fi
OPENSSL_OPTIONS="no-idea no-mdc2 no-rc5 zlib enable-tlsext threads shared"
DEREF_SYMLINKS=deref_symlinks.sh

if [ ! -e /cygdrive/c$ZLIB_PATH ] ; then
	echo "/cygdrive/c$ZLIB_PATH not found"
	echo "Please fix ZLIB_PATH in this script, or export it as an environment"
	echo "var before invocation."
	exit 1
fi

# BUILD
if [ -e $OPENSSL_DIR ] ; then
	echo "$OPENSSL_DIR already exists." 1>&2
	echo "This script refuses to overwrite it" \
	     "(the build probably wouldn't work anyway)" 1>&2
	exit 1
fi
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
