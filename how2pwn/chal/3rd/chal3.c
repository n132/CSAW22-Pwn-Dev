#include <stdio.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <linux/seccomp.h>
#include <linux/filter.h>
#include <sys/prctl.h>
#include <stddef.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
void checkin(){
    char real_ticket[0x30] = {0};
    char your_ticket[0x30] = {0};
    int f = open("./ticket",0);
    read(f,real_ticket,0x20);
    read(0,your_ticket,0x20);
    close(f);
    if(strncmp(real_ticket,your_ticket,0x20))
        _exit(1);
    return ; 
}
void init(){
    fclose(stderr);
    setvbuf(stdin,  0, 2, 0);
    setvbuf(stdout, 0, 2, 0);
    checkin();
}
void sandbox(){
    // I known you guys have known how to use shellcraft to solve my challenges in few seconds
    // I am creating a sandbox to only allow few syscalls so you have to construct the shellcode yourself
    // You can find all about secomp at this page: https://man7.org/linux/man-pages/man2/seccomp.2.html
    struct sock_filter filter[] = {
        BPF_STMT(BPF_LD | BPF_W | BPF_ABS,
                offsetof(struct seccomp_data, nr)),
        BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, __NR_openat, 0, 1),
        BPF_STMT(BPF_RET | BPF_K, SECCOMP_RET_ALLOW),
        BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, __NR_read, 0, 1),
        BPF_STMT(BPF_RET | BPF_K, SECCOMP_RET_ALLOW),
        BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, __NR_writev, 0, 1),
        BPF_STMT(BPF_RET | BPF_K, SECCOMP_RET_ALLOW),
        BPF_STMT(BPF_RET | BPF_K, SECCOMP_RET_KILL_PROCESS),
    };
    struct sock_fprog prog = {
        .len = sizeof(filter) / sizeof(filter[0]),
        .filter = filter,
    };
    // set no_new_privs, or it would return  EACCES:
    syscall(__NR_prctl,PR_SET_NO_NEW_PRIVS, 1,0,0,0);

    // Apply the filter. 
    syscall(__NR_seccomp,SECCOMP_SET_MODE_FILTER,0,&prog);
}
int main(){
    init();
    char buf[0x100]; 
    read(0, buf, 0x100);
    void (* p )(); 
    p = (void (*)())buf;
    sandbox();
    p();
    return 1;
}
