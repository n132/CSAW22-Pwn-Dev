#include<stdio.h>
int main(){
	char buf[1];
	size_t a =0xfffffffffff;
	read(0,buf,a);
}
