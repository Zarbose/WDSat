#!/bin/bash -u

mydir=Documents/WDSat/benchmarks/Rainbow/N_25_M_53
src=$HOME/$mydir

cd ..
i=0
for elm in $src/*;do
    echo "------------- Nouvelle instance "`expr $i + 1`" -------------"
    ./wdsat_solver -S -x -i $elm
    ((i+=1))
done