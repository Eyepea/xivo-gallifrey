#!/bin/sh

# Xivo 0.3 - Upgrade ptitquicc-powerpc-uclibc.
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

PATH=/sbin:/bin:/usr/sbin:/usr/bin

sysrootfile=sysroot.img
sysroot=http://192.168.0.220/cross/dists/ptitquicc-powerpc-uclibc/$sysrootfile
root=/dev/mtdblock2

echo "upgrading firmware, please wait..."
wget --no-check-certificate $sysroot -O $root > /dev/null
echo "firmware upgraded, rebooting..."
reboot
