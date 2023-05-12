#!/bin/bash -u 

workdir="/home/simon/Documents/WDSat"
formules="/home/simon/Documents/WDSat/benchmarks/Rainbow/N_25_M_53"

dest_file="all.csv"

rm $dest_file

i=0
for elm in $formules/*; do
    seed=$(basename $elm | cut -d '_' -f 4)

    echo "------------- Nouvelle instance $seed -------------"
    file=${seed}tmp.txt
    $workdir/wdsat_solver -i $elm > $file &

    pids[${i}]=$!

done

echo "Attente"
for pid in ${pids[*]}; do
    wait $pid
done
echo "Fin attente"



for elm in $formules/*; do
    seed=$(basename $elm | cut -d '_' -f 4)

    echo "------------- Traitement $seed -------------"
    file=${seed}tmp.txt

    conf=$(cat $file | grep "conf" | cut -d ':' -f 2)

    ter=$(cat $file | grep  -E 'ter ' | wc -l)
    subt=$(cat $file | grep  -E 'subt ' | wc -l)
    xor=$(cat $file | grep  -E 'xor ' | wc -l)
    gauss=$(cat $file | grep  -E 'gaus ' | wc -l)

    echo "$seed;$conf;$ter;$subt;$xor;$gauss;" >> $dest_file

done