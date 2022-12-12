#!/bin/bash -u

mydir=$HOME/Documents/WDSat
benchmark=Documents/WDSat/benchmarks/Rainbow/N_25_M_53
src=$HOME/$benchmark
save=0

err() {
    echo "[$(date +'%Y-%m-%d')]: $*" >&2
}

usage() {
    echo -e "\nUse: ./$(basename "$0") [-h] [-S]\n"
    echo -e "Type:
    -h for help
    -S to enable the saving of the results into $src\n"
}

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

cd $mydir
i=0
for elm in $src/*;do
    echo "------------- Nouvelle instance "`expr $i + 1`" -------------"
    if [ $save -eq 0 ];then
        ./wdsat_solver -x -i $elm
    else
        ./wdsat_solver -S -x -i $elm
    fi
    ((i+=1))
done