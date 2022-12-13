#!/usr/bin/python3
# -*- coding: utf-8 -*-

from statistics import mean, pstdev
import sys

def saveEnvironment():
    return "../../result/Rainbow/experimentation_in_progress/big_tables/all_calc.txt"


if len( sys.argv ) < 2:
    print( "\tusage: python3 calc.py path_to_file" )
    exit()

strparam = sys.argv[1]


def calcul():
    list_conf=[]
    list_activation=[]
    list_temps=[]
    list_moyenne=[]
    list_ecartype=[]
    

    with open(strparam, 'r') as f:
        line = f.readline()
        while line != "":
            line=line.split(";")
            # print(line)
            list_conf.append(line[0])
            list_activation.append(line[1])
            list_temps.append(line[2])
            line=f.readline()

    list_conf=list(map(float,list_conf))
    list_activation=list(map(float,list_activation))
    list_temps=list(map(float,list_temps))

    # print(list_conf)
    # print(list_activation)
    # print(list_temps)

    list_moyenne.append(mean(list_conf))
    list_moyenne.append(mean(list_activation))
    list_moyenne.append(mean(list_temps))

    list_ecartype.append(pstdev(list_conf))
    list_ecartype.append(pstdev(list_activation))
    list_ecartype.append(pstdev(list_temps))

    conf_ml=float(list_moyenne[0]/list_moyenne[2])

    return list_moyenne,list_ecartype,conf_ml

def save_calcul(vars):
    i=0
    with open(saveEnvironment(), 'a') as f:
        # f.write("FILE_NAME;"+strparam+"\n")
        for elm in vars:
            if (i == 0):
                # to_write="MOYENNE;"
                to_write=""
                for cpt in range(len(elm)):
                    # print(elm[cpt])
                    if cpt == len(elm)-1:
                        to_write+=str(elm[cpt])+"\n"
                    else:
                        to_write+=str(elm[cpt])+";"
                # print(to_write)
                f.write(to_write)

            if (i == 1):
                # to_write="ECARTYPE;"
                to_write=""
                for cpt in range(len(elm)):
                    # print(elm[cpt])
                    if cpt == len(elm)-1:
                        to_write+=str(elm[cpt])+"\n"
                    else:
                        to_write+=str(elm[cpt])+";"
                # print(to_write)
                f.write(to_write)

            if (i == 2):
                # to_write="CONF_ML;"
                to_write=""
                to_write+=str(elm)+";;\n"
                # print(to_write)
                f.write(to_write)
                
            i+=1
        f.write("---------------\n")


var=calcul()
save_calcul(var)
# print(var)