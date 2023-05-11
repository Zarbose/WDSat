#!/bin/bash -u

cat tmp.txt | grep "classique" > classique.csv
cat tmp.txt | grep "xorgauss" > xorgauss.csv
cat tmp.txt | grep "backtrack" > backtrack.csv
