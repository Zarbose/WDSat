import sys
import os

def usage():
    print( "\tusage: python3 stat.py source_dir" )

def err():
    print("error : python3 stat.py")


if len( sys.argv ) < 2:
    err()
    print( "Bad usage" )
    usage()
    exit(1)
elif len( sys.argv ) == 2:
    path_dir=str(sys.argv[1])

    if os.path.exists(path_dir) == False:
        err()
        print( "Bad arguments :",path_dir, ": no such file or directory")
        usage()
        exit(1)

    os.system("echo 'test' > tmp.txt")

    print("ICI",os.path.exists(path_dir))