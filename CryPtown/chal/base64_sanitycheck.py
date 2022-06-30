import base64
import os
import random
from pwn import *
# context.log_level = 'debug'
for x in range(0x1000):
    s = os.urandom(random.randint(1,0xfff))
    res = base64.b64encode(s)
    p = process("./base64")
    p.send(s)
    data  = p.readline()[:-1]
    if(data!=res):
        print("?")
        print(data)
        print(res)
        exit(1)
    p.close()
print("[+] Success")