#!/bin/bash

bash_args="--login -c"
make_args="make all -w -C `readlink -f .` -f Makefile.ghc"

if [ -z "$MSYS32" ]; then MSYS32=/e/msys32; fi
if [ -z "$MSYS64" ]; then MSYS64=/e/msys64; fi

set -x

MSYSTEM=MING64 winpty $MSYS64/usr/bin/bash.exe $bash_args "PATH=/mingw64/bin:\$PATH $make_args"
MSYSTEM=MING32 winpty $MSYS32/usr/bin/bash.exe $bash_args "PATH=/mingw32/bin:\$PATH $make_args"

tar cvzf ghc-jailbreak.tar.gz i686/ x86_64/
