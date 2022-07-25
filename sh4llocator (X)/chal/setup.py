#!/usr/bin/python
from pathlib import Path
import subprocess
pwd = Path(".") 

def call(cmd,cwd):
    return subprocess.check_call(cmd,shell=True,cwd=cwd)

def init():
    name = 'bash-9439ce094c9aa7557a9d53ac7b412a23aa66e36b'
    target_dir= Path('.') / name
    if(not target_dir.exists()):
        call(f"tar xf ./{name}.tar.gz",pwd)
    call("patch -R -p4 < ../diff",target_dir)
    call(f"cp ./sh4llocator.c ./{name}",pwd)
    call("./configure",target_dir)
    call("make -j`nproc`",target_dir)
    call("cp ./bash ../sh4llocator",target_dir)
    print("[+] Init")

def clean():
    call("rm -rf ./bash-9439ce094c9aa7557a9d53ac7b412a23aa66e36b",pwd)
    if(0):  
        call("rm -rf ./sh4llocator",pwd)
    
import sys
def usage():
    print("Usage:\n\tpython3 ./setup.py [ init | ]")
if __name__ == "__main__":
    if(len(sys.argv)!=2):
        usage()
    else:
        if sys.argv[1] == 'init':
            init()
        elif sys.argv[1] == "clean":
            clean()
