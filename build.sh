#!/bin/sh
# This is my first attempt to write a shell!

set -x

if [ ! -d "./Build" ];then
        mkdir ./Build
fi

if [ ! -d "./LogFiles" ];then
        mkdir ./LogFiles
fi

cd ./Build \
        && cmake ..\
        && make