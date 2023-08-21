#!/bin/bash -u 

### This script calculates N and M using o2 for rainbow systems

[ $# -lt 1 ] && echo -e "Usage :\n      ./$(basename "$0") o2" && exit 1

o2=$1

n=$(expr $o2 - 2)
m=$(expr \( $o2 \* 2 \) - 1)

echo "n = $n m = $m"