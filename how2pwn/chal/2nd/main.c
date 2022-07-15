#include<stdio.h>
#define STDIN 0
int main(){
    char buf[0x100]; 
    read(STDIN, buf, 0x10); // Sorry I am too lazy to print an additional "0"
    void (* p )(); 
    p = buf;
    p();
    return 1;
}
