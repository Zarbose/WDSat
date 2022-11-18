#!/bin/bash

err() {
    echo "[$(date +'%Y-%m-%d')]: $*" >&2
}

usage() {
    echo -e "\nUse: ./$(basename "$0") [-h] [-d]\n"
    echo -e "Type:
    -h for help
    -d for debug\n"
}

debug=0

### Gestion des paramètres
options=$(getopt -o hd -l help -- "$@")
eval set -- "$options" # eval for remove simple quote

while true; do
    case "$1" in 
        -h|--help) 
            usage
            exit 0
            shift;;
        -d)
            debug=1
            shift;;
        --)
            shift
            break;;
        *) 
            usage;
            shift;;
    esac 
done


while ((cpt<10));do
    if [ $debug == 1 ];then
        ./wdsat_solver -x -i benchmarks/MQ/25_1.anf
    else
        result_cmd=$(./wdsat_solver -x -i benchmarks/MQ/25_1.anf)
    fi
    ((cpt+=1))
    echo $cpt
done


