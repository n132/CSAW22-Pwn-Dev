#!/usr/bin/python3
from pathlib import Path
import subprocess
def call(cmd,cwd):
    return subprocess.check_call(cmd,shell=True,cwd=cwd)

def clean():
    print("[+] Clean")

def release():
    call("make",Path("."))
    # copy chal1
    call("cp ./public/bin/all/chal1 ./public/bin/chal")
    # copy source code
    call("cp ./chal/*/*.c ./public/src/")
    print("[+] Release")

import sys
if __name__ == "__main__":
    if(len(sys.argv)==1):
        print("Usage: python3 ./setup  [ release ]")
    elif len(sys.argv)==2:
        if(sys.argv[1]=="release"):
            release()
        elif sys.argv[1]=='clean':
            clean()
        else:
            print("Usage: python3 ./setup [ release ]")
    else:
        print("Usage: python3 ./setup [ release ]")
