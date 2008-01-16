#!/bin/sh

set -e

file="$1"
realfile="`readlink -f "$file"`"
rm "$file"
cp "$realfile" "$file"
