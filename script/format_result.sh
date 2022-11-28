#!/bin/bash -u

err() {
    echo "[$(date +'%Y-%m-%d')]: $*" >&2
}

usage() {
    echo -e "\nUse: ./$(basename "$0") [-h] file\n"
    echo -e "Type:
    -h for help\n"
}

[ $# -lt 1 ] && err 'Missing arguments, type -h' && exit 1

### Gestion des paramètres
options=$(getopt -o hd -l help -- "$@")
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

full_path=$1

nb_ligne=$(wc -l $full_path | cut -d ' ' -f 1)
nb_ligne=$(($nb_ligne - 1))

header=$(head -n 1 $full_path)
tail -n $nb_ligne $full_path | sort -t ';' -n -k 1 -r

