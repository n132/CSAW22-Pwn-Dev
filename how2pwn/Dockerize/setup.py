import os
from pathlib import Path
import subprocess
def call(cmd,cwd):
    return subprocess.check_call(cmd,shell=True,cwd=cwd)
    
def init_chal_dir(stage,dir):
    call("cp ../public/ctf.xinetd .",dir)
    call("cp ../public/Dockerfile .",dir)
    call("cp ../public/docker-compose.yml .",dir)
    call("cp ../public/start.sh .",dir)
    bin_dir = dir / "bin"
    call(f"cp ../public/bin/OtherStages/{stage} .",bin_dir)
    call(f"cp ../public/bin/OtherStages/{stage} .",bin_dir)
def chal1():
    work_dir = Path(".") / "chal1"
    
    try:
        work_dir.mkdir(exist_ok=False)
    except:
        print("[-] Fail to mkdir")
        exit(1)
    
    
    

def clean():
    call("rm -rf ./chal1")


if __name__ == "__main__":
    init_chal_dir();
    # chal1()
#    clean()