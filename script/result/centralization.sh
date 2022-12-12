#!/bin/bash -u

home_project=Documents/WDSat
mydir_unit_tables=result/Rainbow/experimentation_in_progress/to_analyse
mydir_big_tables=result/Rainbow/experimentation_in_progress/big_tables
mydir_dest=result/Rainbow/experimentation_complete

src_unit_tables=$HOME/$home_project/$mydir_unit_tables
src_big_tables=$HOME/$home_project/$mydir_big_tables
dest=$HOME/$home_project/$mydir_dest


name_dir=`date +%d-%m-%Y_%H:%M:%S`
if [ ! -e $dest/$name_dir ];then
    mkdir $dest/$name_dir $dest/$name_dir/unit_tables $dest/$name_dir/big_tables
fi

cp -r $src_unit_tables/* $dest/$name_dir/unit_tables
cp -r $src_big_tables/* $dest/$name_dir/big_tables

