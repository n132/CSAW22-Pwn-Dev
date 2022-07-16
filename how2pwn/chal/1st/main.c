#include<stdio.h>
#define STDIN 0
int main(){
    // A buffer is created to store your shellcode
    char buf[0x100]; 
    read(STDIN, buf, 0x100);
    // A function point is defined and points to the buffer.
    void (* p )(); 
    p = buf;
    // Let's run the shellcode
    p();
    return 1;
}
