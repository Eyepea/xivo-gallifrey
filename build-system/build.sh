#!/bin/bash

# Copyright (C) 2006 Richard Braun <rbraun@proformatique.com>
# 
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
# 
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

set -e

source config

for file in include/*; do
  source $file
done

if [ $# -ne 1 ]; then
  echo "usage: $0 <distribution>"
  echo
  echo "See dists/ for supported distributions."
  exit 1
fi

DIST=$1
source dists/$DIST
source archs/$ARCH
source targets/$TARGET
init_buildenv
clean_prefix
make_toolchain
make_busybox
make_zlib
make_libbz2
make_libpng
make_libjpeg
make_dropbear
make_iptables
make_ncurses
make_openssl
make_wget
make_asterisk
make_php
make_vsftpd
make_sysconf
make_sysimgs
