#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/mman.h>

#include <malloc/shmalloc.h>


// Got the idea from https://pwnable.tw/#37, although I didn't solve it.
typedef struct node{
    char *key;
    char *val;
    struct node *next;
} node;

typedef struct inode{
    size_t type; // 0 -> dir 1-> file
    char * name;
    struct inode *ptr[0x10];
    char * content;
} inode;

typedef struct pnode{
    inode * ptr;
    struct pnode * next;
} pnode;

node * variables = NULL;
inode * home = NULL;
inode * cur_dir = NULL;

void panic(const char *s){
    puts(s);
    exit(1);
}
int check_name( char * c){
    for(char *i = c ; *i != 0 ; i++)
    {
        if(*i < 0x30 || *i > 0x7f)
            if(*i != 0x2e)
                panic("Improper name");
    }    
    return 1;
}
void insert(inode * dir,inode * item){
    // for(int i=0;i<0x10;i++)
    // {
    //     if(dir->ptr[i]==0)
    //     {
    //         dir->ptr[i] = item;
    //         return ;
    //     }
    // }
    // exit(1);
    /*vul*/
    int i = 0  ; 
    for(; i < 0x10; i ++)
        if(dir->ptr[i]==0)
            break;
    dir->ptr[i] = item;
    return ;
}
inode * pwd(){
    return cur_dir;
}
pnode * PATH  = 0 ;
pnode* new_pnode(){
    pnode *tmp = (pnode *)malloc(sizeof(pnode));
    tmp->next = 0 ;
    tmp->ptr = pwd();
    return tmp;
}

void push_current_pnode(){
    pnode * tmp = PATH;
    while(tmp->next)
        tmp = tmp->next;
    tmp->next = new_pnode();
} 

void cd(char *dir_name){
    if(!strcmp("",dir_name))
    {
        pnode * next, * ptr;
        ptr = PATH;
        while(ptr->next)
        {
            next = ptr->next;
            free(ptr);
            ptr = next;
        }
        free(ptr);
        PATH = cur_dir = home;
        push_current_pnode();
        
    }
    else if(!strcmp("..",dir_name))
    {
        
        pnode * pre, * tmp;
        pre = pwd();
        if(!strcmp(pre->ptr->name,"~"))
            return ; 
        
        pre = 0 ; 
        tmp  = PATH; 
        while(tmp->next)
        {
            pre = tmp;
            tmp = tmp->next;
        }
        pre->next = 0 ; 
        free(tmp);
        cur_dir = pre->ptr;
    }
    else{
        inode *cur = pwd();
        for(int i=0;i<0x10;i++)
        {
            if(cur->ptr[i]==0 || cur->ptr[i]->type==1)
                continue;
            if(!strcmp(dir_name,cur->ptr[i]->name))
            {
                cur_dir = cur->ptr[i];
                push_current_pnode();
            }
        }

    }
}
inode * inode_init(int type,char *cmd){
    inode * p = (inode *)malloc(sizeof(inode));
    p->type = type;
    if(check_name(cmd))
    {
        p->name = strdup(cmd);
        for(int i=0;i<0x10;i++)
            p->ptr[i]=0;
        
        if(type){
            p->content = malloc(0x18);
            memset(p->content,0,0x18);
        }
        else
           p->content = 0;
        
        inode *cur = pwd();
        insert(cur,p);
        return p;
    }
    exit(1);
}
void mk_dir(char *cmd){
    if(!strcmp("",cmd))
        exit(-1);
    inode_init(0,cmd);
    return ;
}
inode * touch(char *cmd){
    if(!strcmp("",cmd))
        exit(0);
    return inode_init(1,cmd);
}
void rm_file(char *name, inode *cur_path){
    if(!strcmp("",name))
        return ;
    inode *cur = cur_path;
    for(int i=0;i<0x10;i++)
    {
        if(cur->ptr[i]==0 || cur->ptr[i]->type==0)
            continue;

        if(!strcmp(name,cur->ptr[i]->name))
        {
            inode * target = cur->ptr[i];
            free(target->name);
            if(target->content!=0)
                free(target->content);
            free(target);
            cur->ptr[i] = 0;

            return ;
        }
    }
    return ;
}
void rm(char *cmd){
    rm_file(cmd,pwd());
}
void rm_dir(char *name, inode *cur_path){
    if(!strcmp("",name))
        return ;
    inode *cur = cur_path;
    int i = 0;
    for(i = 0;i<0x10;i++)
    {
        if(cur->ptr[i]==0 || cur->ptr[i]->type==1)
            continue;
        if(!strcmp(name,cur->ptr[i]->name))
        {
            cur = cur->ptr[i];
            break;
        }
    }
    if(i==0x10) // dir doesn't exist
        return ;
    // Find the target and start to delete the contents
    for(int i=0;i<0x10;i++)
    {
        if(cur->ptr[i]==0)
            continue;
        if(cur->ptr[i]->type==1)
            rm_file(cur->ptr[i]->name,cur);
        else if(cur->ptr[i]->type==1)
            rm_dir(cur->ptr[i]->name,cur);
        else
            exit(1);

    }
    return ;
}
void rmdir_(char *cmd){
    rm_dir(cmd,pwd());
}
void ls_dir(inode * target){
    for(int i=0;i<0x10;i++)
    {
        if(target->ptr[i]!=0)
            printf("%s\t",target->ptr[i]->name);
    }
    puts("");
}
void ls(char *cmd){
    inode * cur = pwd();
    inode * target = 0;
    if(!strcmp("",cmd))
        target = pwd();
    else
    {
        for(int i=0;i<0x10;i++)
        {
            if(cur->ptr[i]==0 || cur->ptr[i]->type==1)
                continue;

            if(!strcmp(cmd,cur->ptr[i]->name))
            {
                target = cur->ptr[i];
                break;
            }
        }
    }
    if(target==0)
        exit(1);
    ls_dir(target);
    return;
}
inode * lookfor(char *filename){
    inode * cur = pwd();
    for(int i=0;i<0x10;i++)
    {
        if(cur->ptr[i]==0 || cur->ptr[i]->type==0)
            continue;
        if(!strcmp(filename,cur->ptr[i]->name))
            return cur->ptr[i];
        
    }
    return 0 ; 
}
void echo(char *c){
    char *filename = 0;
    inode * target = 0 ;
    filename = strstr(c, " > ");

    if(filename){
        *filename = 0;
        filename+=3;
        target  = lookfor(filename);
        if(target==0)
            target = touch(filename);

    }
    if(c[0] == 0x24) // $ echo a varible, -> to a file or to stdout
    {
        node * ptr = variables;
        while(ptr)
        {
            if(!strcmp(c+1,ptr->key))
            {
                if(target)
                {
                    if(target->content)
                        free(target->content);
                    target->content = strdup(ptr->val);
                }
                else
                    printf("%s\n",ptr->val);
                return ;
            }
            ptr = ptr->next;
        }
        if(!target)
            puts("");
    }
    else // echo a string > to a file or to stdout
    {
        if(target)
        {
            if(target->content)
                free(target->content);
            target->content = strdup(c);
        }
        else
            printf("%s\n",c);
        return ;
    }
}
void declare(char *key,char *val){
    node * ptr = (node *)malloc(sizeof(node));
    if(ptr<=0)
        exit(1);
    size_t key_len = strlen(key);
    size_t val_len = strlen(val);
    ptr->key = (char *)malloc(key_len+1);
    ptr->val = (char *)malloc(val_len+1);
    if(ptr->key <= 0 || ptr->val <= 0)
        exit(1);
    memset(ptr->key,0,key_len+1);
    memset(ptr->val,0,val_len+1);
    strcpy(ptr->key,key);
    strcpy(ptr->val,val);
    ptr->next = variables;
    variables = ptr;
}
void unset(char *c){
    node * ptr = variables;
    node * pre = 0;
    while(ptr)
    {
        if(!strcmp(c,ptr->key))
        {
            node * next = ptr->next;
            free(ptr->val);
            free(ptr->key);
            free(ptr);
            if(pre==0)
                variables = next;
            else
                pre->next = next;
            return ;
        }
        pre = ptr;
        ptr = ptr->next;
    }
}
void show_pwd(){
    inode * cur = pwd();
    printf("%s\n",cur->name);
    return;
}
void cat(char *filename){
    inode * cur = pwd();    
    for(int i=0;i<0x10;i++)
    {
        if(cur->ptr[i]==0 || cur->ptr[i]->type==0)
            continue;
        if(!strcmp(filename,cur->ptr[i]->name))
        {
            printf("%s\n",cur->ptr[i]->content);
            return ; 
        }
    }
    printf("File doesn't exist\n");
}
void sh4ll(){
    printf("# ");
    size_t buffer_size = 0x400;
    char* cmd = (char *)malloc(buffer_size);
    if(cmd<=0)
        exit(1);
    memset(cmd,0,buffer_size);
    fgets(cmd,buffer_size-1,stdin);

    char* tmp = strstr(cmd,"\n");
    if(tmp!=0)
        *tmp = 0;
    
    char* pos = strstr(cmd," ");
    if( pos == NULL )
    {
        char* pos = strstr(cmd,"=");
        if(pos==NULL)
        {
            if(!strcmp("ls",cmd))
                ls("");
            else if(!strcmp("cd",cmd))
                cd("");
            else if(!strcmp("pwd",cmd))
                show_pwd();
            else if(!strcmp("exit",cmd))
                exit(0);
            else
                printf("Command not found: %s\n",cmd);
            
        }
        else{
            *pos = 0 ;
            declare(cmd,pos+1);
        }
    }
    else{
        * pos = 0;
        if(!strcmp("echo",cmd)){
            echo(pos+1);
        }
        else if(!strcmp("unset",cmd)){
            unset(pos+1);
        }
        else if(!strcmp("mkdir",cmd)){
            mk_dir(pos+1);
        }
        else if(!strcmp("rmdir",cmd)){
            rmdir_(pos+1);
        }
        else if(!strcmp("touch",cmd)){
            touch(pos+1);
        }
        else if(!strcmp("rm",cmd)){
            rm(pos+1);
        }
        else if(!strcmp("ls",cmd)){
            ls(pos+1);
        }
        else if(!strcmp("cd",cmd)){
            cd(pos+1);
        }
        else if(!strcmp("pwd",cmd)){
            show_pwd();
        }
        else if(!strcmp("exit",cmd)){
            exit(0);
        }
        else if(!strcmp("cat",cmd)){
            cat(pos+1);
        }
        else{
            printf("Command not found: %s\n",cmd);
        }
    }
    free(cmd);
    return ;
}
void init_path(){
    if(PATH)
        return ; 
    PATH = new_pnode();
}
void init_home(){
    home = (inode *)malloc(sizeof(inode));
    if(home<=0)
        panic("Init Home");
    home->type = 0;
    home->name = strdup("~");
    for(int i=0;i<0x10;i++)
        home->ptr[i]=0;
    home->content = 0;
    cur_dir = home;
    return ; 
}
void logo_loader(){
    char buf[0x500]={0}; 
    int f = open("./logo",0);
    if(f<0)
        panic("Open error");
    size_t res = read(f,buf,sizeof(buf));
    if(res>=sizeof(buf))
        panic("Read error");
    puts(buf);
}
void init(){
    fclose(stderr);
    setvbuf(stdin,  0, 2, 0);
    setvbuf(stdout, 0, 2, 0);
    logo_loader();
    init_home();
    init_path();
}
int chal(){
    init();
    while(1)
        sh4ll();
    return 0 ; 
}