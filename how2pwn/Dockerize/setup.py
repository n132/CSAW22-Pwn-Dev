import os
from pathlib import Path
import subprocess
def call(cmd,cwd):
    return subprocess.check_call(cmd,shell=True,cwd=cwd)
    
def init_chal_dir(stage,cwd):
    call(f"cp ../public/ctf.xinetd ./{stage}",cwd)
    call(f"cp ../public/Dockerfile ./{stage}",cwd)
    call(f"cp ../public/docker-compose.yml ./{stage}",cwd)
    call(f"cp ../public/start.sh ./{stage}",cwd)
    bin_dir = cwd / stage / "bin"
    bin_dir.mkdir()
    call(f"cp ../public/bin/OtherStages/{stage} ./{stage}/bin/",cwd)
    call(f"cp ../chal/{stage}/flag ./{stage}/bin/",cwd)
    if(Path(f"../chal/{stage}/ticket").exists()):
        call(f"cp ../chal/{stage}/ticket ./{stage}/bin/",cwd)
    
def chal1():
    work_dir = Path(".") / "chal1"
    
    try:
        work_dir.mkdir(exist_ok=False)
    except:
        print("[-] Fail to mkdir")
        exit(1)
    
    init_chal_dir("chal1",Path("."))
    
    
    
    

def clean():
    call("rm -rf ./chal1")


if __name__ == "__main__":
    
    chal1()
#    clean()