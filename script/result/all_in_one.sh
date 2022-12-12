#!/bin/bash -u

mydir=Documents/WDSat/result/Rainbow
src=$HOME/$mydir


for elm in $src/*; do
    ! [ -f $elm ] && continue;
    cat $elm >> $src/all_file.txt
    echo >> $src/all_file.txt
done