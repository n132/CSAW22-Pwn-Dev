#include<string.h>
#include<stdio.h>
#include<stdlib.h>
typedef struct node{
    char *key;
    char *val;
    struct node *next;
} node;

typedef struct inode{
    //file=0, folder=1, tail guard=2
    size_t type;
    //file/folder name
    char * name;
    //file=content, folder=NULL
    char * content;
    //file=NULL, folder=linked list of files
    struct inode * files;
    //single/last item=NULL, linked list=next
    struct inode * next;
    //file=-1,folder=filecount
    size_t filecount;
} inode;
node * variables = NULL;
inode * root = NULL;

void mkdir(char *cmd)
{
    node * cmds = (node *)malloc(sizeof(node));
    node * curr_cmd = cmds;
    if(cmds<=0)
        _Exit(1);
    printf("%s\n", cmd);
    char delim[] = "/";
    char *ptr = strtok(cmd, delim);
    while (ptr != NULL)
    {
        curr_cmd->key = NULL;
        size_t val_len = strlen(ptr);
        curr_cmd->val = (char *)malloc(val_len+1);
        strcpy(curr_cmd->val, ptr);
        curr_cmd->next = (node *)malloc(sizeof(node));
        curr_cmd = curr_cmd->next;
        ptr = strtok(NULL, delim);
    }
    curr_cmd->key = NULL;
    curr_cmd->val = NULL;
    curr_cmd->next = NULL;

    //test function
    curr_cmd = cmds;
    while(curr_cmd > 0 && curr_cmd->val > 0)
    {
        printf("%s\n", curr_cmd->val);
        curr_cmd = curr_cmd->next;
    }
}

void declare(char *key,char *val)
{
    node * ptr = (node *)malloc(sizeof(node));
    if(ptr<=0)
        _Exit(1);
    size_t key_len = strlen(key);
    size_t val_len = strlen(val);
    ptr->key = (char *)malloc(key_len+1);
    ptr->val = (char *)malloc(val_len+1);
    if(ptr->key <= 0 || ptr->val <= 0)
    _Exit(1);
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
    else{
        printf("%s",c);
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
        _Exit(1);
    memset(cmd,0,buffer_size);
    fgets(cmd,buffer_size,stdin);

    char* tmp = strstr(cmd,"\n");
    if(tmp!=0)
    {
        *tmp = 0;
    }
    char* pos = strstr(cmd," ");
    if(pos == NULL)
    {
        char* pos = strstr(cmd,"=");
        if(pos==NULL)
        {
            printf("command not found: %s",cmd);
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
        else{
            printf("command not found: %s",cmd);
        }
    }
    free(cmd);
    return ;
}
void init(){};
void usage(){};
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
    usage();
    shell();
}
