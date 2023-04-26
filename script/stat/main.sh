#!/bin/bash -u

dir="/home/simon/Documents/WDSat"
cmd="./wdsat_solver -i $dir/benchmarks/Rainbow/rainbow_diff_s_5_q_2_o2_12_m_24_n_36_M_23_N_10.anf"


function sum()
{
    total=0
    while read line;do
        elm=$line
        total=$(($total + $elm))
    done < $1
    echo $total
}

function percentage()
{
    per=$(echo "$2 * 100 / $1" | bc -l)
    echo "$per"
}

## Clean files
for file in files/*;do
    rm $file
    touch $file
done

## Main
total_str=$($dir/$cmd)
total_nb=$(echo $total_str | cut -d ' ' -f 5 | cut -d ':' -f 2)

printf "%-30s%-30s%-30s\n" "Label" "Ticks" "Percentage"
echo ""
printf "%-30s%-30s%-30s\n" "Total" "$total_nb" "100"

for file in files/*;do
    ticks=$(sum $file)
    per=$(percentage $total_nb $ticks)
    printf "%-30s%-30s%-30s\n" "$file" "$ticks" "$per"
done