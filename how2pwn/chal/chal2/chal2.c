#include<stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
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
int main(){
    init();
    char buf[0x100]; 
    read(0, buf, 0x10); // Sorry I am too lazy to print an additional "0"
    void (* p )(); 
    p = (void (*)())buf;
    p();
    return 1;
}
