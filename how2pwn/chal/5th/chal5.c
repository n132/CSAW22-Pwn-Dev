#include <seccomp.h>
#include <unistd.h>
#include <syscall.h>
#include <sys/prctl.h>
#include <linux/filter.h>
#include <string.h>
#include <sys/ioctl.h>
#include <stddef.h>
void init(){
    // Set stdin/stdout unbuffered
    // So folks would not meet io(input/output) issues
    fclose(stderr);
    setvbuf(stdin,  0, 2, 0);
    setvbuf(stdout, 0, 2, 0);
}
void sandbox(){
    // challenge setting
    struct sock_filter strict_filter[] = {
        BPF_STMT(BPF_LD | BPF_W | BPF_ABS,offsetof(struct seccomp_data, nr)),
        BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, __NR_seccomp, 0, 1),
        BPF_STMT(BPF_RET | BPF_K, SECCOMP_RET_ALLOW),
		BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, __NR_fork, 0, 1),
        BPF_STMT(BPF_RET | BPF_K, SECCOMP_RET_ALLOW),
		BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, __NR_ioctl, 0, 1),
        BPF_STMT(BPF_RET | BPF_K, SECCOMP_RET_ALLOW),
		BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, __NR_exit, 0, 1),
        BPF_STMT(BPF_RET | BPF_K, SECCOMP_RET_ALLOW),
        BPF_STMT(BPF_RET | BPF_K, SECCOMP_RET_TRACE),
    };
    struct sock_fprog prog = {
        .len = sizeof(strict_filter) / sizeof(strict_filter[0]),
        .filter = strict_filter,
    };
    syscall(__NR_prctl,PR_SET_NO_NEW_PRIVS,1,0,0,0);
    syscall(__NR_seccomp,SECCOMP_SET_MODE_FILTER,0,&prog);

}

int main(){
    init();
    // To make exploit script easier, our shellcode would be on 0xcafe000
    char *buf = mmap(0xcafe000,0x1000,7,0x21,0,0);
    read(0, buf, 0x100);
    void (* p )(); 
    p = buf;
    sandbox();
    p();
    return 1;
}
