#!/bin/bash -u

source ../color.conf

mydir=Documents/WDSat/testing
src=$HOME/$mydir

while read line; do
    first_file=$src/$line"_before.txt"
    second_file=$src/$line"_after.txt"
    if ! [ -e $first_file ];then
        echo "Creating file : "$first_file
        touch $first_file
    fi
    if ! [ -e $second_file ];then
        echo "Creating file : "$second_file
        touch $second_file
    fi
done < list_vars.txt

echo -e "Generating files ${GREEN}done${NC}"