#!/bin/sh

# Xivo 0.2 - Upgrade Xivo.
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
sysroot=http://192.168.0.220/cross/dists/xivo/$sysrootfile

root=$(ls -l /dev/root | cut -d '>' -f 2)

# Trim spaces.
root=$(echo $root)

echo "upgrading firmware, please wait..."

if [ "$root" = "/dev/hdc2" ]; then
  newroot="/dev/hdc3"
elif [ "$root" = "/dev/hdc3" ]; then
  newroot="/dev/hdc2"
else
  echo "root partition is different from what was expected, aborting."
  exit 1
fi

wget --no-check-certificate $sysroot -O $newroot > /dev/null
mkdir /tmp/hdc1
mount -o sync /dev/hdc1 /tmp/hdc1
sed -ie "s%$root%$newroot%" /tmp/hdc1/boot/grub/menu.lst
umount /tmp/hdc1

echo "firmware upgraded, rebooting..."
reboot
