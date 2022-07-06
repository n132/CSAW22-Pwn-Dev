#include<string.h>
#include<stdio.h>
#include<stdlib.h>
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

node * variables = NULL;
inode * home = NULL;
inode * cur_dir = NULL;
int check_name()
{
    return 1;
}
void insert(inode * dir,inode * item){
    for(int i=0;i<0x10;i++)
    {
        if(dir->ptr[i]==0)
        {
            dir->ptr[i] = item;
            return ;
        }
    }
    exit(1);
}
inode * pwd(){
    return cur_dir;
}
void cd(char *dir_name){
    if(!strcmp("",dir_name))
        cur_dir = home;
    else{
        inode *cur = pwd();
        for(int i=0;i<0x10;i++)
        {
            if(cur->ptr[i]==0 || cur->ptr[i]->type==1)
                continue;
            if(!strcmp(dir_name,cur->ptr[i]->name))
            {
                cur_dir = cur->ptr[i];
            }
        }
    }
}
void inode_init(int type,char *cmd)
{
    inode * p = (inode *)malloc(sizeof(inode));
    p->type = type;
    if(check_name(cmd))
    {
        p->name = strdup(cmd);
        for(int i=0;i<0x10;i++)
            p->ptr[i]=0;
        p->content = 0;
        inode *cur = pwd();
        insert(cur,p);
        return ;
    }
    exit(1);
}
void mkdir(char *cmd){
    if(!strcmp("",cmd))
        exit(-1);
    inode_init(0,cmd);
    return ;
}
void touch(char *cmd)
{
    if(!strcmp("",cmd))
        return ;
    inode_init(1,cmd);
    return ;
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
void rmdir(char *cmd){
    rm_dir(cmd,pwd());
}
void ls_dir(inode * target)
{
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
void declare(char *key,char *val)
{
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
void echo(char *c)
{

    if(!strncmp(c,"$",1))
    {
        // TODO: write to file
        if(strstr(c, " > "))
        {
            printf("%s\n", ">! unfinished function");
            inode * dir = pwd();

        }
        c = c+1;
        node * ptr = variables;
        while(ptr)
        {
            if(!strcmp(c,ptr->key))
            {
                printf("%s\n",ptr->val);
                return ;
            }
            ptr = ptr->next;
        }
    }

    else if(strstr(c, " > "))
    {

        //write to file
        // printf("%s",c);
        char * pos = strstr(c, " > ");
        * pos = 0;
        char * content = c;
        char * dir_name = pos + 3;
        printf("content: %s\n", content);
        printf("filename: %s\n", dir_name);
        inode * cur = pwd();
        //TODO: make findfile into function
        for(int i=0;i<0x10;i++)
        {
            if(cur->ptr[i]==0 || cur->ptr[i]->type==0)
                continue;
            if(!strcmp(dir_name,cur->ptr[i]->name))
            {
                cur = cur->ptr[i];
                break;
            }
        }

        printf("Found file: %s/n", cur->name);
        cur->content = content;
        printf("Filecontent: %s\n", cur->content);
    }
    else
    {
        printf("%s\n",c);
    }
    return ;
}
void unset(char *c)
{
    node * ptr = variables;
    while(ptr)
    {
        node * pre = 0;
        if(!strcmp(c,ptr->key))
        {
            node * next = ptr->next;
            free(ptr->val);
            free(ptr->key);
            free(ptr);
            if(pre==0)
            {
                variables = next;
            }
            else{
                pre->next = next;
            }
            return ;
        }
        ptr = ptr->next;
    }
}
void sh4ll()
{
    size_t buffer_size = 0x400;
    char* cmd = (char *)malloc(buffer_size);
    if(cmd<=0)
        exit(1);
    memset(cmd,0,buffer_size);
    fgets(cmd,buffer_size-1,stdin);

    char* tmp = strstr(cmd,"\n");
    if(tmp!=0)
    {
        *tmp = 0;
    }
    char* pos = strstr(cmd," ");
    if( pos == NULL )
    {
        char* pos = strstr(cmd,"=");
        if(pos==NULL)
        {
            if(!strcmp("ls",cmd))
                ls("");
            else{
                printf("command not found: %s\n",cmd);
            }
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
            mkdir(pos+1);
        }
        else if(!strcmp("rmdir",cmd)){
            rmdir(pos+1);
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
        else{
            printf("command not found: %s\n",cmd);
        }
    }
    free(cmd);
    return ;
}
void init_home()
{
    home = (inode *)malloc(sizeof(inode));
    if(home<=0)
        exit(1);
    home->type = 0;
    home->name = strdup("~");
    for(int i=0;i<0x10;i++)
        home->ptr[i]=0;
    home->content = 0;
    cur_dir = home;
}
void    panic(const char *s)
{
    puts(s);
    exit(1);
}
int     secure_open(const char * fname){
    int f = open(fname,0);
    if(f<0)
        panic("Open error");
    return f;
}
void logo_loader(){
    char buf[0x500] ; 
    memset(buf,0,sizeof(buf));
    int f =  secure_open("./logo");
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
}
void usage(){;
}
void shell()
{
    while(1)
    {
        printf("# ");
        sh4ll();
    }
}
int main()
{
    init();
    init_home();
    usage();
    shell();
}
