#include<stdio.h>
#include<stdint.h>
#include<fcntl.h>
#include<stdlib.h>
#include<unistd.h>
#include<utils.c>
double R = 0.32;
size_t L = 0x400;

// Utils 
void panic(const char *s)
{
    puts(s);
    exit(1);
}
int readint(){
    char buf[0x10];
    int tmp  = read(0,buf,0xf);
    if(tmp<=0)
        panic("Read error");
    return atoi(buf);
}
size_t readn(char *ptr, size_t len){
  int tmp = read(0, ptr, len);
  if ( tmp <= 0 )
    panic("Read error");
  if ( ptr[tmp - 1] == 10 )
    ptr[tmp - 1] = 0;
  return tmp;
}
uint8_t urand_byte(int f)
{
    uint8_t r;
    read(f,&r,1);
    return r;    
}
// ------------------------------------------------------------

typedef struct random_bytes
{
    size_t *pos;
    size_t cur;
    size_t limit; 
} random_bytes; 

typedef struct key_struct
{
    random_bytes *rb;
    // random character positions 2**16 may result uncontrolable result
    uint8_t  *key;
    size_t key_len;
    // a string
} key_struct; 

key_struct KList[0x10];

void * secure_malloc(size_t size)
{
    void * p = malloc(size);
    if(p<=0)
        panic("Malloc error");
    return p ; 
}
void * secure_realloc(void * p, size_t size)
{
    p = realloc(p,size);
    if(p<=0)
        panic("Realloc error");
    return p ; 
}
key_struct * key_init(uint8_t *key,size_t kl)
{
    key_struct *k = 0 ;
    k       = secure_malloc(sizeof(key_struct));
    k->rb   = secure_malloc(sizeof(random_bytes));
    k->rb->pos = 0;
    k->rb->cur = 0;
    k->rb->limit = 0;
    k->key = key;
    k->key_len = kl;
    return k; 
}
void enc(uint8_t *plaintext, key_struct * key, uint8_t *key, size_t key_len)
{
    int f       = 0;
    uint8_t * p = 0;
    size_t ct   = 0;
    size_t rct  = 0;
    uint8_t * e = 0;
    
    f   = open("/dev/urandom",0,0);
    p   = plaintext; 
    e   = secure_realloc(0,L);
    if( f<0)
        panic("enc");

    key_struct *k = key_init(key,key_len);
    
    while( p[ct-rct]!= 0)
    {
        if(urand_byte(f)<= R * 0x100)
        {
            if(k->rb->cur >= k->rb->limit)
            {

                k->rb->pos = secure_realloc(k->rb->pos,sizeof(size_t) * (k->rb->limit+0x20));
                k->rb->limit +=0x20;
            }
            k->rb->cur++;
            k->rb->pos[rct++] = ct ;
            e[ct++] = urand_byte(f); // Insert a random byte
        }
        else{
            e[ct++] = k->key[(ct-rct)%key_len] ^ p[ct-rct];
        }
    }
    close(f);
    for(int i =0 ; i < ct;i++)
    {
        printf("%d\n",e[i]);
    }
}
void dec(uint8_t *ciphertext,key_struct *key)
{
    printf("TODO");
}
void logo()
{
    ;
}
void init()
{
    fclose(stderr);
    setvbuf(stdin, 0LL, 2, 0LL);
    setvbuf(stdout, 0LL, 2, 0LL);
    alarm(120);//I'll set the alarm in set-up.sh so I can remove this.
    logo();
}
void menu()
{
    puts("=========================");
    puts("0. Key Management");
    puts("1. Encode");
    puts("2. Decode");
    puts("3. Challenge this Encryption Scheme");
    puts("4. Leave");
    puts(" ========================= ");
}
void key_list(){
    puts(" ========================= ");
    for(int i = 0 ; i<0x10; i++)
    {
        if(KList[i])
        {
            printf(" Key[ %d ], len = %d \n",i,KList[i].key_len);
        }
    }
    puts(" ========================= ");
}
int key_insert(key_struct * k)
{
    if(!k)
        panic("Invalid data");
    for(int i = 0 ; i < 0x10 ; i++)
    {
        if(!KList[i])
        {
            KList[i] = k ; 
            return 0;
        }
    }
    return 1; 
}
void key_destructor(key_struct * k){
    if(!k)
        return;
    if(k->rb)
        free(k->rb);
    free(k->key);
    free(k);
    return;
}
void key_del(){
    key_list();
    printf("Which key: ");
    size_t idx = readint() ;
    if(idx >= 0x10)
        panic("Invalid data");
    if(!KList[idx])
        panic("Invalid data");
    key_destructor(KList[idx]);
    KList[idx] = 0 ;
    puts("[+] Key Del Done");
    
}
void key_gen(){
    size_t key_len = 0;
    printf("Size: ");
    key_len = readint() ;
    if(key_len == 0  || key_len > 0x18)
        panic("Invalid data");
    uint8_t * key = secure_malloc(key_len) ;
    printf("Key string: ");
    readn(key,key_len);
    key_struct k = key_init(key,key_len);
    if(key_insert(k)){
        puts("[-] Fail to Gen a New Key");
        key_destructor(k);
    }
    else{
        puts("[+] Key Gen Done");
    }
}
void key_menu(){
    puts("=========================");
    puts("0. Add a Key");
    puts("1. Del a Key");
    puts("2. Show all Keys");
    puts("3. Return");
    puts(" ========================= ");
}
void key_management()
{
    while(1)
    {
        key_menu();
        switch (readint())
        {
        case 0:
            key_gen();
            break;
        case 1:
            key_del();
            break;
        case 2:
            key_list();
            break;
        default:
            return ;
        }
    }
}
void encode(){
    key_list();
    printf("Which key do you want to use: ");
    size_t idx = readint();
    if(idx >= 0x10)
        panic("Invalid data");
    if(!KList[idx])
        panic("Invalid data");
}
void decode(){;}
void challge(){;}
void play_ground()
{
    while(1)
    {
        menu();
        switch (readint())
        {
        case 0:
            key_management();
            break;
        case 1:
            encode();
            break;
        case 2:
            decode();
            break;
        case 3:
            challge();
            break;
        default:
            exit(0);
        }
    }
}
void test(){
    uint8_t p[]= {1,2,3,4,5,6,67};
    uint8_t k[]= {0};
    enc(p,k,sizeof(k));
}
int main()
{
    init();
    play_ground();

}