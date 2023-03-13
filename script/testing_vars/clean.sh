#!/bin/bash -u

source $HOME/Documents/WDSat/script/color.conf

MYDIR=Documents/WDSat/testing
src=$HOME/$MYDIR

for file in $src/*;do 
    > $file
done

# echo -e "Cleaning files ${GREEN}done${NC}"