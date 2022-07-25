#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
void logo_loader(){
    char buf[0x300] ; 
    memset(buf,0,sizeof(buf));
    int f =  open("./logo",0);
    if(f<0)
        exit(1);
    size_t res = read(f,buf,sizeof(buf));
    if(res>=sizeof(buf))
        exit(1);
    puts(buf);
}
void init(){
    fclose(stderr);
    setvbuf(stdin,0,2,0);
    setvbuf(stdout,0,2,0);
    logo_loader();
}

void menu(){
    puts("======= CSAW'22 =======");
    puts("[1] \tCreate a note");
    puts("[2] \tDelete a note");
    puts("[3] \tRead a note");
    puts("[4] \tEdit a note");
    printf("> ");
}
int readint(){
    char buf[0x20]={0};
    read(0,buf,0x1f);
    return atoi(buf);
}
typedef struct note
{
    char *ptr;
    size_t type;
}note;

note* Note[0x10]={0};

void add_note(int secret){
    puts("Which page do you want to write the note down?");
    unsigned int idx =readint();
    if(idx>=0x10)
        exit(1);
    note * p = malloc(sizeof(note));
    if( p <= 0 )
        exit(1);
    if(secret)
    {
        p->type=0xcafebabe;
        p->ptr = malloc(0x8);
        if(p->ptr<=0)
            exit(1);
        puts("Content:");
        read(0,p->ptr,8);
        Note[idx] = p ;
        puts("I'll keep this secret!");
    }
    else{  
        p->type=0;
        puts("How many bytes will it be?");
        size_t len = readint();
        p->ptr = malloc(len);
        if(p->ptr<=0)
            exit(1);
        puts("Content:");
        read(0,p->ptr,len);
        Note[idx] = p ;
    } 
    return ;
}
void add()
{
    puts("Do you want to tell me a secret?(0/1)");
    int res = readint();
    add_note(res);
    return;
}
size_t urand(){
    int f = open("/dev/urandom",0);
    if(f<0)
        exit(1);
    size_t res = 0 ; 
    read(f,&res,sizeof(res));
    return res;
}
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
unsigned int T=2;
void show(){
    puts("Which note do you want to read?");
    unsigned int idx = readint();
    if(idx<0x10 && Note[idx]!=0)
    {
        if(Note[idx]->type)
        {
            if(T==0)
            {
                puts("NO! I am an honest notebook!");
                return ; 
            }
            else if(T==2)
            {
                puts("I'll not do it again. This is the last time\n");
                size_t plain = *((size_t *)Note[idx]->ptr);
                printf("Secret %p\n",(void *)plain);
                
            }
            else if(T==1){
                puts("I could only give you some hints.");
                printf("Applying safe-linking+ ...\n");
                size_t data = *((size_t *)Note[idx]->ptr);
                long int base = (urand()>>28);
                printf("Secret %p(off= %lx)\n", (void *)(base ^ data), base-(data>>12));
            }
            T--;
        }
        else
            puts("It's not a secret, you should known it <3");
    }
    else
        puts("The note doesn't exist.");
}
int main(){
    init();
    int cmd = 0 ; 
    while(1){
        menu();
        cmd = readint();
        switch (cmd)
        {
            case 1:
                add();
                break;
            case 2:
                del();
                break;
            case 3:
                show();
                break;
            default:
                exit(0);
        }
    }
}
