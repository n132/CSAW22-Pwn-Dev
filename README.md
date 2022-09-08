# easyROP

## 0x00 chal

This is an easy ROP challenge. I try to make the easy ROP interesting and educational so I write the program in a reverse way: people would use programming language to write the program  and exploit buffer overflow with ROP. Basically, ROP it's a chain of code pieces and we can use it to write a program!

For people don't have pwn-ing background they may feel weird: "What the heck? Can't believe I can compile this program." After that, they start to learn ROP and know how my challenge works. After understanding the ROP and the challenge, they'll find the vulnerability and use ROP to attack the program!

## 0x01 solution

The there is a ROP chain in the vul function, and it would read 0x200 bytes to buffer whose size is 0x100 so we can overflow it and modify the return address. 

There are two steps in my EXP script
- Leak libc base address and return to main to reuse the vulnerability
- Hijack the program to run `system("/bin/sh")`


## 0x02 EXP

[sol][1]

# how2pwn

## 0x00 chal
how2pwn is a series of educational challenges(4 stages). I write this challenge for people have little experience on pwn. If you are new to pwn, don't hesitate to start from this challenge. And there would be many hints in the challenge, I am pretty sure most people, who have learned OS&C Language, could solve it. If you don't have an enviroment to start, please use mine:

`docker push n132/pwn:22.04`

## 0x01 solution

### Challenge 1

Folks should fill the script with the syscall number of execve and hex value of '/bin/sh'.

### challenge 2

People should fill the short script with shellcode to read longer shellcode.

There is an example:
```
mov rdx,0x100
syscall
```

And uncomment this line to send longer shellcode.

`p.send(b"\x90"*len(shellcode)+asm(shellcraft.sh()))`

### challenge 3

Basically, people are supposed to fill the script and ret to x86 mode. After that, they are able to run orw(open read write) with x86 shellcode.


### challenge 4

Players need to fill the script to exploit. The corresponding c code is at the 0x05 section of this [page][3].


## 0x02 EXP

[sol][2]

# unsafe-linking

## 0x00 chal
It's a glibc-heap challenge that uses the latest LTS Glibc(2.35). There is a simple UAF in free which allows people to leak heap_base address, glibc_base address, and stack address. After leaking these important base addresses, the attackers could use heap exploitation to write arbitrary addresses. But unluckily, hooks are removed in Glibc-2.35, they have to use FSOP or ROP to get a shell.

## 0x01 solution

This challenge is a menu-based challenge, there are three features that allow people to interact with memory.

- Create a note
- Delete a note
- Read a note

The vulnerability is in delete

void del(){
    puts("Which note do you want to delete?");
    unsigned idx = readint();
    if(idx<0x10 && Note[idx]!=0)
    {
        free(Note[idx]->ptr);
        free(Note[idx]);
        // Note[idx]= 0 ; 
    }
    else{
        puts("The note doesn't exist.");
    }
}

As you can see, I free a note but forget to clean the pointer which allows people to use it after freeing it.

You can find the exploit script in next section.

Part0: Decode the leaked address, it's a little complex. You can find the source code in function sol. I used z3 to solve it.

Part1: line 80 - line 96 I use UAF to leak heap_base

Part2: line 97 - line 125 I use IO_FILE_leak to leak libc_base and stack address

Part3: line 127 - line 140 I link a fake chunk on the stack to tcache and modify the stack after retrieve it from tcache.

# 0x02 EXP

[sol][4]



[1] ./ezROP/solution/exp.py
[2] ./how2pwn/solution
[3] https://n132.github.io/2022/07/04/S2.html
[4] ./unsafe-linking/solution/exp.py