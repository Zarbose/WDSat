#!/bin/bash -u

mydir=result/Rainbow/experimentation_in_progress/to_analyse
home_project=Documents/WDSat
src=$HOME/$home_project/$mydir

for elm in $src/*;do
    ! [ -f $elm ] && continue;
    python3 calc.py $elm
done

