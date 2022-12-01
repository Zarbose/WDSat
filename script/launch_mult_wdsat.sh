#!/bin/bash -u

MYDIR=Documents/WDSat/benchmarks/Rainbow/N_25_M_53
src=$HOME/$MYDIR

cd ..

for elm in $src/*;do
    echo "------------- Nouvelle instance -------------"
    ./wdsat_solver -S -x -i $elm
done