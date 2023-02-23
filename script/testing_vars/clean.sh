#!/bin/bash -u

source ../color

MYDIR=Documents/WDSat/testing
src=$HOME/$MYDIR

for file in $src/*;do 
    echo > $file
done

echo -e "Cleaning files ${GREEN}done${NC}"