from pwn import *

p = process("./chal2")
context.terminal = ['tmux', 'splitw', '-h', '-F' '#{pane_pid}', '-P']
# You can check all Linux x64 syscalls at this page: https://syscalls64.paolostivanin.com/
# For this challenge, your task is bypassing the sandbox and read the flag
# there are some hints in this passage: https://n132.github.io/2022/07/03/Guide-of-Seccomp-in-CTF.html
# btw, the name of our target is "flag"
context.arch = 'amd64'
shellcode = f'''
mov rax,w
mov rdi,x
mov rsi,y
mov rdx,z
syscall
...
syscall
...
syscall
'''# 
# gdb.attach(p)
shellcode = asm(shellcode)

p.interactive()