#!/bin/sh

set -e

PROGRAM="`readlink -f "$0"`"
PROGRAM_DIR=${PROGRAM%/*}

find -type l -print0 | xargs -0 -n 1 "${PROGRAM_DIR}/deref_one_symlink.sh"
