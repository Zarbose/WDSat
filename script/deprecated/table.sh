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
! [ -e $1 ] && err "Le paramétre n'est pas un fichier" && exit 2

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

### Récupération du chemin et du nom du fichier à lire
full_path=$1
file_name=$(basename -s .txt $1)
file_path=$(dirname $1)

### Récupération du header
nb_ligne=$(wc -l $full_path | cut -d ' ' -f 1)
nb_ligne=$(($nb_ligne - 1))
header=$(head -n 1 $full_path | cut -d ' ' -f 1)

### Création du tableur qui va contenir les résultat
table_to_create=$file_name"-tab.csv"
dir_to_create=$file_path"/"$table_to_create

if ! [ -e  $dir_to_create ];then
    touch $dir_to_create
fi


### Ecriture dans le tableau
echo $header > $dir_to_create
while read line;do
    echo $line | grep "$header" >/dev/null && continue;

    line=$(echo $line | cut -d ' ' -f 1)
    # conf=$(echo $line | cut -d ';' -f 1)
    # nb_activation=$(echo $line | cut -d ';' -f 2)
    # K1=$(echo $line | cut -d ';' -f 3)
    # K2=$(echo $line | cut -d ';' -f 4)
    # K5=$(echo $line | cut -d ';' -f 5)
    # temps_ml=$(echo $line | cut -d ';' -f 6)

    echo $line >> $dir_to_create
done < $full_path
