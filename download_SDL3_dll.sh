#!/bin/sh

curl -L https://github.com/libsdl-org/SDL/releases/download/release-3.2.18/SDL3-devel-3.2.18-mingw.tar.gz > SDL3-devel.tar.gz
mkdir SDL3-devel
tar zxf SDL3-devel.tar.gz -C SDL3-devel --strip-components=1

read -p "Are you on a 32-bit system? (If unsure you're probably on a 64-bit) [Y/n] " yn
case $yn in 
  [Yy]* ) cp -v SDL3-devel/i686*/bin/SDL3.dll ./build
  * ) cp -v SDL3-devel/x86_64*/bin/SDL3.dll ./build
esac

rm -rf SDL3-devel{,.tar.gz}
