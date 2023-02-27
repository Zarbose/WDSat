#!/bin/bash -u

source $HOME/Documents/WDSat/script/color.conf

mydir=Documents/WDSat/testing
src=$HOME/$mydir

# ./generate_file.sh
# ./clean.sh

while read line; do
    first_file=$src/$line"_before.txt"
    second_file=$src/$line"_after.txt"
    diff -aq $first_file $second_file
done < $HOME/Documents/WDSat/script/testing_vars/list_vars.txt

echo -e "Comparing files ${GREEN}done${NC}"