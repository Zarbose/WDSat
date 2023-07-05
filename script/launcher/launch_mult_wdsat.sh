#!/bin/bash -u

source ../constants.sh

err() {
    echo "[$(date +'%Y-%m-%d')]: $*" >&2
}

usage() {
    echo -e "\nUse: ./$(basename "$0") [-h] [-S] o2\n"
    echo -e "Type:
    -h for help
    -S to enable the saving of the results into $src ( o2=27 N=25 M=53 )\n"
}

# o2=$1
o2=27
N=`expr $o2 - 2`
M=`expr \( $o2 \* 2 \) - 1`

# 27 25 53

mydir=$root_dir
benchmark=benchmarks/Rainbow/N_${N}_M_${M}
src=$mydir/$benchmark

### Gestion des paramètres
options=$(getopt -o Sh -l help -- "$@")
eval set -- "$options" # eval for remove simple quote

while true; do
    case "$1" in 
        -h|--help) 
            usage
            exit 0
            shift;;
        --)
            shift
            break;;
        *) 
            usage;
            shift;;
    esac 
done

cd $mydir

i=0
for elm in $src/*;do
    echo "------------- Nouvelle instance "`expr $i + 1`" -------------"

    ./wdsat_solver -x -i $elm

    ((i+=1))
done