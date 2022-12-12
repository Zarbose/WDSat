#!/bin/bash -u

err() {
    echo "[$(date +'%Y-%m-%d')]: $*" >&2
}

usage() {
    echo -e "\nUse: ./$(basename "$0") o2\n"
}

[ $# -lt 1 ] && err "Missing arguments" && usage && exit 1


mydir_dest=benchmarks/Rainbow
mydir_script=script/generate
home_project=Documents/WDSat
src_script=$HOME/$home_project/$mydir_script
dest=$HOME/$home_project/$mydir_dest


o2=$1
N=`expr $o2 - 2`
M=`expr \( $o2 \* 2 \) - 1`

if [ ! -d "$dest/N_${N}_M_${M}" ];then
    echo "Création de : $dest/N_${N}_M_${M}"
    mkdir $dest/N_${N}_M_${M}
elif [ -d "$dest/N_${N}_M_${M}" ];then
    rm -rf $dest/N_${N}_M_${M}/*
fi

echo "Génération de 10 séquences de rainbow"
./generateMultiRainbowPoly.sh 0 10 $o2

echo "Déplacement des séquences générées dans $dest/N_${N}_M_${M}"
./cpSelectedRainbowSystem.sh $o2

echo "Ajout des variables de configuration dans config.h (ne pas oublier de les décommenter)"
var=`python3 create_wdsat_config_rainbow.py $N $M`