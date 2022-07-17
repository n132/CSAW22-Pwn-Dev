#include<stdio.h>
void init(){
    // Set stdin/stdout unbuffered
    // So folks would not meet io(input/output) issues
    fclose(stderr);
    setvbuf(stdin,  0, 2, 0);
    setvbuf(stdout, 0, 2, 0);
}
int main(){
    init();
    char buf[0x100]; 
    read(0, buf, 0x10); // Sorry I am too lazy to print an additional "0"
    void (* p )(); 
    p = buf;
    p();
    return 1;
}
