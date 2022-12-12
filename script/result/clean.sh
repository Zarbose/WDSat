#!/bin/bash -u

home_project=Documents/WDSat
mydir_unit_tables=result/Rainbow/experimentation_in_progress/to_analyse
mydir_big_tables=result/Rainbow/experimentation_in_progress/big_tables

src_unit_tables=$HOME/$home_project/$mydir_unit_tables/*
src_big_tables=$HOME/$home_project/$mydir_big_tables/*


rm -rf $src_unit_tables $src_big_tables