#!/bin/sh

set -e

PATH=/sbin:/bin:/usr/sbin:/usr/bin

sysrootfile=sysroot.img
sysroot=https://192.168.0.220/cross/dists/ptitpc-i386-uclibc/$sysrootfile

root=$(ls -l /dev/root | cut -d '>' -f 2)

# Trim spaces.
root=$(echo $root)

if [ "$root" = "hda2" ]; then
  newroot="/dev/hda3"
elif [ "$root" = "hda3" ]; then
  newroot="/dev/hda2"
else
  echo "root partition is different from what was expected, aborting."
  exit 1
fi

wget -nv --no-check-certificate $sysroot -O $newroot > /dev/null
mount /boot
root="/dev/$root"
sed -ie "s%$root%$newroot%" /boot/boot/grub/menu.lst
umount /boot
