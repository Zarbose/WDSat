#!/bin/bash

src=/home/simon/Documents/WDSat/xorgauss_sys/new_sys

for elm in $src/*; do
    vars=$(head -n 1 $elm)
    N=$(echo $vars | cut -d ' ' -f 3)
    M=$(echo $vars | cut -d ' ' -f 4)

    ## Gen config.h
    python3 /home/simon/Documents/WDSat/script/test_xorgauss/create_wdsat_config_rainbow.py $N $M

    ## Compil
    cd /home/simon/Documents/WDSat/src
    make 2> /dev/null
    echo 

    ## Exec
    cd /home/simon/Documents/WDSat
    # echo "-----------------------------"
    echo $elm

    echo "-----------------------------"
    return_str=$(./wdsat_solver -x -i $elm)
    echo $return_str
    # echo $return_str >> result.txt
    echo "-----------------------------"

    exit 1

done