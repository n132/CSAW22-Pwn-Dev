#include<stdio.h>
#include <unistd.h>
void init(){
    fclose(stderr);
    setvbuf(stdin,  0, 2, 0);
    setvbuf(stdout, 0, 2, 0);
}
int main(){
    init();
    // A buffer is created to store your shellcode
    char buf[0x100]; 
    read(0, buf, 0x100);
    // A function point is defined and points to the buffer.
    void (* p )(); 
    p = buf;
    // Let's run the shellcode
    p();
    return 1;
}
