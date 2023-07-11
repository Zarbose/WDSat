#!/bin/bash -u

err() {
    echo "[$(date +'%Y-%m-%d')]: $*" >&2
}

usage() {
    echo -e "\nUse:\n\t./$(basename "$0") nb_seed_to_generate o2\n"
}

[ $# -lt 2 ] && err "Missing arguments" && usage && exit 1

start_seed=0
nbf=$1
o2=$2

for (( i = 0; i < $nbf; i++));do
    echo `expr $i + 1`
    ./generateRainbowPoly.sh $start_seed $o2
    ((start_seed+=1))
done