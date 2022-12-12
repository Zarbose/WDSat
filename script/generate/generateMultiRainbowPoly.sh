#!/bin/bash -u

err() {
    echo "[$(date +'%Y-%m-%d')]: $*" >&2
}

usage() {
    echo -e "\nUse: ./$(basename "$0") base_seed nb_file_to_generate o2\n"
}

[ $# -lt 3 ] && err "Missing arguments" && usage && exit 1

start_seed=$1
nbf=$2
o2=$3

for (( i = 0; i < $nbf; i++));do
    echo `expr $i + 1`
    ./generateRainbowPoly.sh $start_seed $o2
    ((start_seed+=1))
done