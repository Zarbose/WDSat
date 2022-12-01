#!/bin/bash -u 

[ $# -lt 1 ] && echo -e "\nUsage :./$(basename "$0") o2"

o2=$1

n=`expr $o2 - 2`
m=`expr \( $o2 \* 2 \) - 1`

echo "n = $n m = $m"

if ! [ -e "../benchmarks/Rainbow/N_${n}_M_${m}" ];then
    echo "Création de : ../benchmarks/Rainbow/N_${n}_M_${m}"
    mkdir "../benchmarks/Rainbow/N_${n}_M_${m}"
fi