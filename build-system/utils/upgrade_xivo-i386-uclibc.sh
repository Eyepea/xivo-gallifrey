#!/bin/sh

# Xivo 0.3 - Upgrade xivo-i386-uclibc.
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
sysroot=http://192.168.0.220/cross/dists/xivo-i386-uclibc/$sysrootfile

root=$(ls -l /dev/root | cut -d '>' -f 2)

# Trim spaces.
root=$(echo $root)

echo "upgrading firmware, please wait..."

if [ "$root" = "hdc2" ]; then
  newroot="/dev/hdc3"
elif [ "$root" = "hdc3" ]; then
  newroot="/dev/hdc2"
else
  echo "root partition is different from what was expected, aborting."
  exit 1
fi

wget -nv --no-check-certificate $sysroot -O $newroot > /dev/null
mount /boot
root="/dev/$root"
sed -ie "s%$root%$newroot%" /boot/boot/grub/menu.lst
umount /boot

echo "firmware upgraded, rebooting..."
reboot
