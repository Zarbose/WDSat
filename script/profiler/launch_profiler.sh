#!/bin/bash -u

err() {
    echo "[$(date +'%Y-%m-%d')]: $*" >&2
}

usage() {
    echo -e "\nUse: ./$(basename "$0") [-h] [-x] input_file\n"
    echo -e "Type:
    -h for help
    -x to activate xorgauss\n"
}

### Constantes
xorgauss=0
input_file=$1
mydir=$HOME/Documents/WDSat
dest_dir=$(date +%D-%T)


### Gestion des paramètres
options=$(getopt -o xh -l help -- "$@")
eval set -- "$options" # eval for remove simple quote

while true; do
    case "$1" in 
        -h|--help) 
            usage
            exit 0
            shift;;
        -x) 
            xorgauss=1
            shift;;
        --)
            shift
            break;;
        *) 
            usage;
            shift;;
    esac 
done
! [ -e $mydir/$input_file ] && usage && exit 2


### Main
mkdir -p $mydir/performance/profiler/$dest_dir

string_options=""

if [ $xorgauss -eq 1 ];then
    string_options="-x -i $input_file"
else
    string_options="-i $input_file"
fi


cd $mydir

valgrind --tool=callgrind ./wdsat_solver $string_options
callgrind_annotate --threshold=99.8 callgrind.out.* > $mydir/performance/profiler/$dest_dir/callgrind.txt
rm callgrind.out.*

echo

valgrind --tool=cachegrind ./wdsat_solver $string_options
cg_annotate --threshold=0.05 cachegrind.out.* > $mydir/performance/profiler/$dest_dir/cachegrind.txt
rm cachegrind.out.*
