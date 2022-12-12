#!/usr/bin/python3
### Ce script permet de transfomer un fichhier sous la forme GF(2) des challenges 'Fukuoka MQ Challenge' en forme anf
import itertools
import numpy as np

def path_file():
    # return 'test'
    return '/home/simon/Documents/WDSat/benchmarks/toy_example/ToyExample-type1-n20-seed0'

def dest_file():
    return path_file()+'.anf'

def format_string(line):
    tmp_str=line.replace(" ", "")
    tmp_str=line.replace("\n", "")
    tmp_str=line.replace("\r", "")
    tmp_str=line.replace("\t", "")
    tmp_str=line.split(":")
    return tmp_str

def extract_param():
    i=0
    n=0
    m=0
    file = open(path_file(), "r")
    line = file.readline()
    while line:
        if i < 7:
            if line.find('Number of variables (n) :') != -1:
                tmp=format_string(line)
                n=int(tmp[1])
            if line.find('Number of polynomials (m) :') != -1:
                tmp=format_string(line)
                m=int(tmp[1])
        else:
            return([n,m])
        i+=1
        line = file.readline()
    file.close()
    return -1

def find_duplicate(list,val):
    val=val.split(" ")
    for i in range(len(list)):
        elm=list[i]
        elm=elm.split(" ")
        if (val[1] == elm[0] and val[0] == elm[1]):
            return i
    return -1

def supp_duplicate(list,val):
    test=find_duplicate(list,val)
    if test != -1:
        return list.pop(test)

def gen_seq_X(n):
    seq_X=[]

    tmp_seq=[]
    tmp_combi_seq=[]
    for i in range(n):
        tmp_seq.append(str(i+1))
    tmp_seq.append('1')
    seq_X.append(tmp_seq)

    for v in itertools.combinations(tmp_seq, 2):
        to_app=v[0]+" "+v[1]
        tmp_combi_seq.append(to_app)
    
    seq_X.append(tmp_combi_seq)

    for elm in seq_X[1]:
        supp_duplicate(seq_X[1],elm)

    for i in range(n):
        seq_X[1].append(str(i+1)+"d"+"2")

    return seq_X

def init_vector(val,n):
    tmp=val.split(" ")
    vec=[]
    for i in range(n):
        vec.append(0)

    if val.count('d') != 1:
        vec[int(tmp[0])-1]=1
        vec[int(tmp[1])-1]=1
    else:
        tmp=val.split("d")
        vec[int(tmp[0])-1]=2
    
    vec = np.array(vec)
    return vec


def compare_monomial(alpha,beta,n):
    if alpha == beta:
        return 1

    vector_alpha=init_vector(alpha,n)
    vector_beta=init_vector(beta,n)
    sub_vector = vector_alpha-vector_beta

    cpt=0
    for i in range(len(sub_vector)):
        if sub_vector[i] != 0:
            cpt=i

    if sub_vector[cpt] < 0: # alpha < beta
        return 1
    else: # alpha > beta
        return 0
    

def max_value(list,n):
    max=0
    for i in range(len(list)):
        if(compare_monomial(list[i],list[max],n)) == 1:
            max=i

    return max

def sort_seq(list,n):
    to_sort=list
    sorted=[]

    while len(to_sort) > 0:
        id_max = max_value(to_sort,n)
        sorted.append(to_sort[id_max])
        to_sort.pop(id_max)

    return sorted

def remove_undesirable_elm(list):
    for i in range(len(list)):
        if list[i].count('d') == 1:
            tmp=list[i].split("d")
            list[i]=tmp[0]

    return list

def clean_destination():
    destination = open(dest_file(),'w')
    destination.write("")
    destination.close()

def add_header(n,m):
    destination = open(dest_file(),'a')
    destination.write("p cnf "+str(n)+" "+str(m)+'\n')
    destination.close()

def find_degree_monomial(elm):
    tmp=elm.split(" ")
    return len(tmp)

def generate_anf(seq_X):
    i=0

    print(seq_X)

    file = open(path_file(), "r")
    destination = open(dest_file(),'a')

    line = file.readline()
    while line:
        if i < 7:
            i+=1
        else:
            tmp_str=line.split(" ")
            to_write="x"
            for i in range(len(tmp_str)-1):
                if tmp_str[i] == '1':
                    degree=find_degree_monomial(seq_X[i])
                    if degree == 1:
                        to_write=to_write+" "+str(seq_X[i])
                    else:
                        to_write=to_write+" ."+str(degree)+" "+str(seq_X[i])
            destination.write(to_write+' 0\n')
        line = file.readline()

    file.close()
    destination.close()

    return 0


param=extract_param()
seq=gen_seq_X(param[0])
seq_sort=sort_seq(seq[1],param[0])
# print(seq_sort)
# print(seq[0])
seq_sort=remove_undesirable_elm(seq_sort)
clean_destination()
add_header(param[0],param[1])
generate_anf(seq_sort+seq[0])