#!/bin/bash -u

MYDIR=Documents/mq-comparison-suite
src=$HOME/$MYDIR/

err() {
    echo "[$(date +'%Y-%m-%d')]: $*" >&2
}

usage() {
    echo -e "\nUse: ./$(basename "$0") seed o2\n"
}

[ $# -lt 2 ] && err "Missing arguments" && usage && exit 1

let val1=$2*2
let val2=$2*3
let val3=$val1-1
let val4=$2-2
file=$src/systems/rainbow_diff_s_${1}_q_2_o2_${2}_m_${val1}_n_${val2}_M_${val3}_N_${val4}.anf
if [ -f "$file" ]; then
	echo "Polynomial system of seed $1 and o2 $2 already exist !"
else
	cd $src
	if [[ $# -eq 3 ]]; then
		echo "Launching command : sage rainbow_attacks.sage --seed $1 --q 2 --o2 $2 --gen_only"
	fi
	sage rainbow_attacks.sage --seed $1 --q 2 --o2 $2 --gen_only
	cd systems/
	ls . | grep s_$1_q_2_o2_$2 |
	while read -r line; do
		case $line in
			*".anf")
				;;
			*)
				if [[ $# -eq 3 ]]; then
					echo "Deleting $line"
				fi
				rm $line
				;;
		esac
	done
fi

exit 0
