#!/bin/bash -u

mydir=result/Rainbow/experimentation_in_progress/to_analyse
home_project=Documents/WDSat
src=$HOME/$home_project/$mydir

position_information=7

for elm in $src/*;do 
    val=$(cat $elm | cut -d ';' -f $position_information | sort -n | head -n 1)
    echo $val
done > prof.txt