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
        call("tar xf ./.tar",pwd)
    

if __name__ == "__main__":
    init()
