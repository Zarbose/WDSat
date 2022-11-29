### Ce script permet de transfomer un fichhier sous la forme GF(2) des challenges 'Fukuoka MQ Challenge' en forme anf

def path_file():
    return 'challenge-1-55-0'

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

def gen_seq_X(n,m):
    seq_X=[]
    for i in range(n):
        seq_X.append(str((i+1)))


    seq_X.append('1')
    return seq_X



param=extract_param()
print(gen_seq_X(param[0],param[1]))