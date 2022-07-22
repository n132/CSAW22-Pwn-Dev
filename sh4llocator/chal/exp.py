from pwn import *
context.log_level='debug'
context.arch='amd64'
context.terminal = ['tmux', 'split', '-w', '-F' '#{pane_pid}', '-P']
p=process('./sh4llocator')
ru 		= lambda a: 	p.readuntil(a)
r 		= lambda n:		p.read(n)
sla 	= lambda a,b: 	p.sendlineafter(a,b)
sa 		= lambda a,b: 	p.sendafter(a,b)
sl		= lambda a: 	p.sendline(a)
s 		= lambda a: 	p.send(a)

def cmd(c):
    sla("# ",c)

cmd("tocuh n132")

#gdb.attach(p)


p.interactive()
