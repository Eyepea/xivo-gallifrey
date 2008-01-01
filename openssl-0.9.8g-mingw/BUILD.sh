#!/bin/sh

set -e

tar xzvf openssl-0.9.8g.tar.gz 
cd openssl-0.9.8g
patch -p1 < ../build_under_broken_cygwin.patch
patch -p1 < ../build_cygwin_gnu_make_3_81_cross_mingw.patch
patch -p1 < ../build_broken_cross_mingw_out_implib.patch
patch -p1 < ../build_no_at_in_Makefile.patch
patch -p1 < ../build_shared_set_x.patch
../deref_symlinks.sh
PERL="c:/cygwin/bin/perl" ./Configure mingw -L/cygwin/home/Administrateur/zlib-1.2.3/ -I/cygwin/home/Administrateur/zlib-1.2.3/ no-idea no-mdc2 no-rc5 zlib enable-tlsext threads shared 
../deref_symlinks.sh
make --debug=j PERL="c:/cygwin/bin/perl"
