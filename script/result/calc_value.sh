#!/bin/bash -u

mydir=Documents/WDSat/result/Rainbow/to_analyse
src=$HOME/$mydir

for elm in $src/*;do
    ! [ -f $elm ] && continue;
    python3 calc.py $elm
done

