#!/bin/bash -u 

workdir="/home/simon/Documents/WDSat"
formules="/home/simon/Documents/WDSat/benchmarks/Rainbow/N_25_M_53"

dest_file="all.csv"

rm $dest_file

i=0
for elm in $formules/*; do
    seed=$(basename $elm | cut -d '_' -f 4)

    echo "------------- Nouvelle instance $seed -------------"
    $workdir/wdsat_solver -i $elm > tmp.txt

    # cat tmp.txt

    conf=$(cat tmp.txt | grep "conf" | cut -d ':' -f 2)
    temps_ml=$(cat tmp.txt | grep "temps_ml" | cut -d ':' -f 2)
    ticks=$(cat tmp.txt | grep "ticks" | tr '.' ',' | cut -d ':' -f 2)

    echo "$seed;$conf;$temps_ml;$ticks;" >> $dest_file

    # ./split.sh
    # exit 0
done