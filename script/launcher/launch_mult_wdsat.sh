#!/bin/bash -u

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

mydir=$HOME/Documents/WDSat
benchmark=Documents/WDSat/benchmarks/Rainbow/N_${N}_M_${M}_V2
src=$HOME/$benchmark
save=0

### Gestion des paramètres
options=$(getopt -o Sh -l help -- "$@")
eval set -- "$options" # eval for remove simple quote

while true; do
    case "$1" in 
        -h|--help) 
            usage
            exit 0
            shift;;
        -S) 
            save=1
            shift;;
        --)
            shift
            break;;
        *) 
            usage;
            shift;;
    esac 
done
counter=0
cd $mydir
i=0
for elm in $src/*;do
    echo "------------- Nouvelle instance "`expr $i + 1`" -------------"

    # if [ $counter -eq 10 ];then
    #     sleep 270
    # fi

    # if [ $counter -eq 20 ];then
    #     sleep 270
    # fi

    # ./wdsat_solver -S -x -i $elm &
    ./wdsat_solver -S -x -i $elm

    # counter=$((counter+1))

    ((i+=1))
done