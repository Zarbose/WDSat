#!/bin/bash -u

mydir=result/Rainbow/experimentation_in_progress/big_tables
home_project=Documents/WDSat
src=$HOME/$home_project/$mydir

[ -e $src/all_calc.txt ] && rm $src/all_calc.txt 2> /dev/null
./calc_value.sh
# ./centralization.sh
# ./clean.sh