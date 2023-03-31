import sys

def usage():
    print( "\tusage: python3 create_wdsat_config_rainbow.y N M" )

def err():
    print("error : create_wdsat_config_rainbow.py")

def configAlreadyExist(N,M):
    with open("../../src/config.h", 'r') as file:
        line=file.readlines()
        for elm in line:
            if elm.find("Rainbow : N={0} M={1}".format(N,M)) != -1:
                return True
    return False


if len( sys.argv ) < 3:
    err()
    print( "Bad usage" )
    usage()
    exit(1)
elif len( sys.argv ) == 3:
    N=int(sys.argv[1])
    M=int(sys.argv[2])

    if (M < N):
        err()
        print( "Bad arguments M < N" )
        usage()
        exit(1)

    if configAlreadyExist(N,M) == False:
        with open("../../src/config.h", 'a') as f:
            f.write("""
/** Rainbow : N={0} M={1} **/
/*# define __MAX_ANF_ID__ {2} // make it +1
# define __MAX_DEGREE__ 3 // make it +1
# define __MAX_ID__ {3}
# define __MAX_BUFFER_SIZE__ 200000
# define __MAX_EQ__ {5}
# define __MAX_EQ_SIZE__ 4 //make it +1
# define __MAX_XEQ__ {1}
# define __MAX_XEQ_SIZE__ {4}*/\n""".format(N, M, N + 1, int(N*(N+1)/2), N * (N + 1), int(M*(M-1)/2)))
