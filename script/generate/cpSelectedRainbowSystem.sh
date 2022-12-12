#!/bin/bash -u

err() {
    echo "[$(date +'%Y-%m-%d')]: $*" >&2
}

usage() {
    echo -e "\nUse: ./$(basename "$0") o2\n"
}

[ $# -lt 1 ] && err "Missing arguments" && usage && exit 1

o2=$1
N=`expr $o2 - 2`
M=`expr \( $o2 \* 2 \) - 1`

mydir_src=benchmarks/Rainbow/new_seq
mydir_dest=benchmarks/Rainbow/N_${N}_M_${M}
home_project=Documents/WDSat

src=$HOME/Documents/mq-comparison-suite/systems/.
dest=$HOME/$home_project/$mydir_dest



for elm in $src/*;do
    echo $elm | grep "^.*.o2_${o2}_.*.$" > /dev/null && echo $elm && cp $elm $dest
done