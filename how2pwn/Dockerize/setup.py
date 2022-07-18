#!/usr/bin/python3
from pathlib import Path
import subprocess
def call(cmd,cwd):
    return subprocess.check_call(cmd,shell=True,cwd=cwd)
    
def init(stage,cwd):
    call(f"cp ../public/ctf.xinetd ./{stage}",cwd) 
    call(f"cp ../public/Dockerfile ./{stage}",cwd) 
    call(f"cp ../public/docker-compose.yml ./{stage}",cwd)

    with open(cwd/stage/"docker-compose.yml",'r') as f:
        data = f.read()
    data = data.replace("60001",str(60000+int(stage[-1])))
    with open(cwd/stage/"docker-compose.yml",'w') as f:
        f.write(data)
    
    call(f"cp ../public/start.sh ./{stage}",cwd) 
    bin_dir = cwd / stage / "bin"
    bin_dir.mkdir()
    call(f"cp ../public/bin/OtherStages/{stage} ./{stage}/bin/chal", cwd)
    call(f"chmod +x ./{stage}/bin/chal",cwd)
    call(f"cp ../chal/{stage}/flag ./{stage}/bin/", cwd)
    if(Path(f"../chal/{stage}/ticket").exists()):
        call(f"cp ../chal/{stage}/ticket ./{stage}/bin/",cwd)
    
def builder(stage="chal1"):
    work_dir = Path(".") / stage
    try:
        work_dir.mkdir(exist_ok=False)
    except:
        print("[-] Fail to mkdir")
        exit(1)
    init(stage,Path("."))
    print(f"[+] Built {stage}")

def clean():
    call("rm -rf ./chal1 ./chal2 ./chal3 ./chal4 ./chal5",Path("."))
    print("[+] Clean Done")

def build():
    builder("chal1")
    builder("chal2")
    builder("chal3")
    builder("chal4")
    builder("chal5")
def up():
    cur = Path(".")
    for x in range(5):
        cwd = cur/("chal"+str(x+1))
        call("docker-compose build",cwd)
        subprocess.Popen("docker-compose up".split(" "),cwd=cwd)
        print("[+] chal"+str(x+1)+" up")
            
    print("Up Done")
def down():
    cur = Path(".")
    for x in range(5):
        cwd = cur/("chal"+str(x+1))
        subprocess.Popen("docker-compose stop".split(" "),cwd=cwd)
        print("[+] chal"+str(x+1)+" down")
            
    print("Down Done")
import sys
if __name__ == "__main__":
    if(len(sys.argv)==1):
        print("Usage: python3 ./setup  [ build, clean, up, down ]")
    elif len(sys.argv)==2:
        if(sys.argv[1]=="build"):
            build()
        elif sys.argv[1]=='clean':
            clean()
        elif sys.argv[1]=='up':
            up()
        elif sys.argv[1]=='down':
            down()
        else:
            print("Usage: python3 ./setup [ build | clean ]")
    else:
        print("Usage: python3 ./setup  [ build, clean, up, down ]")