#!/bin/bash -u

source $HOME/Documents/WDSat/script/color.conf

mydir=Documents/WDSat/testing
src=$HOME/$mydir

# ./generate_file.sh
# ./clean.sh

while read line; do
    first_file=$src/$line"_before.txt"
    second_file=$src/$line"_after.txt"
    result=$(diff -aq $first_file $second_file)
    nb_w=$(echo $result | wc -w)
    if [ $nb_w -gt 1 ];then
        echo $result
        $HOME/Documents/WDSat/script/testing_vars/clean.sh
        exit 2
    fi
done < $HOME/Documents/WDSat/script/testing_vars/list_vars.txt

echo -e "Comparing files ${GREEN}done${NC}"

$HOME/Documents/WDSat/script/testing_vars/clean.sh

exit 0