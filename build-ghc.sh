#!/bin/bash

bash_args="--login -c"
make_args="make all -w -C `readlink -f .` -f Makefile.ghc"

x86=/e/msys32/usr/bin/bash.exe
x64=/e/msys64/usr/bin/bash.exe

set -x

MSYSTEM=MING64 winpty $x64 $bash_args "PATH=/mingw64/bin:\$PATH $make_args"
MSYSTEM=MING32 winpty $x86 $bash_args "PATH=/mingw32/bin:\$PATH $make_args"

tar cvzf ghc-jailbreak-0.5.tar.gz i686/ x86_64/