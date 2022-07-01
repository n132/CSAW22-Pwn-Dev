#include <stdio.h>
#include <stdint.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "base64.h"
#include "cryptown.h"
#include "utils.h"

key_struct *KList[0x10];
void base64_output(char *buf,size_t len){
    size_t size = Base64encode_len(len);
    char *tmp = secure_malloc(size);
    memset(tmp,0,size);
    int out_len = Base64encode(tmp,buf,len);
    if(size < out_len || out_len<=1)
        panic("oob");
    puts(tmp);
    free(tmp);
}
key_struct * key_init(uint8_t *key,size_t kl){
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
void enc(uint8_t *plaintext, size_t plaintext_len , key_struct * k){
    int f       = 0;
    uint8_t * p = 0;
    size_t ct   = 0;
    size_t rct  = 0;
    uint8_t * e = 0;
    size_t e_len_max = plaintext_len;

    f   = secure_open("/dev/urandom");
    p   = plaintext; 
    e   = secure_realloc(0, e_len_max);
    
    if(k->rb)
    {
        if(k->rb->pos)
            free(k->rb->pos);
        free(k->rb);
        k->rb = secure_malloc(sizeof(random_bytes));
        memset(k->rb,0,sizeof(random_bytes));
    }
    while( ct-rct < plaintext_len )
    {
        if(urand_byte(f)<= R * 0x100)
        {
            if(k->rb->cur >= k->rb->limit)
            {
                if( rct > plaintext_len)
                    puts("Meaningless Setting, why don't you use OTP?"); 
                else{
                    k->rb->limit +=0x20;
                    k->rb->pos = secure_realloc(k->rb->pos,sizeof(size_t) * k->rb->limit);
                }
            }
            k->rb->cur++;
            k->rb->pos[rct++] = ct ;
            e[ct++] = urand_byte(f); // append a random byte
        }
        else{
            e[ct++] = k->key[(ct-rct)%(k->key_len)] ^ p[ct-rct];
        }
        
        if(ct>=e_len_max)
        {
            if(ct>=2*plaintext_len)
                puts("Meaningless Setting, why don't you use OTP?");
            e = secure_realloc(e,e_len_max*2);
            e_len_max *= 2 ;
        }
    }
    
    close(f);
    puts("Ciphertext:");
    base64_output(e,ct);
    free(e);
}
void dec(uint8_t *ciphertext, size_t c_len, key_struct *key){
    size_t ct               = 0 ; 
    size_t kl               = key->key_len;
    uint8_t * key_string    = key->key;
    uint8_t * raw           = secure_malloc(Base64decode_len(ciphertext));
    size_t raw_len          = Base64decode(raw,ciphertext);
    if(!raw_len)
        panic("Decode");
    puts("Plaintext:");
    for(int i=0 ; i< raw_len-1 ;i++)
    {
        if( ct < key->rb->cur);
        {
            if(i==key->rb->pos[ct])
            {
                ct++;
                continue;
            }
        }
        uint8_t n = raw[i] ^ key_string[(i-ct)%kl];
        write(1,&n,1);
    }
    free(raw);
}
void menu(){
    puts(" =========================");
    puts("0. Key Management");
    puts("1. Encode");
    puts("2. Decode");
    puts("3. Challenge");
    puts("4. Leave");
    puts(" ========================= ");
    printf("> ");
}
void key_list(){
    int tmp = 1 ; 
    puts(" ************************* ");
    
    for(int i = 0 ; i<0x10; i++)
    {
        if(KList[i])
        {
            printf(" Key[ %d ], len = %lu \n",i,KList[i]->key_len);
            tmp = 0;
        }
    }
    if(tmp==1)
        puts("\tNone");

    puts(" ************************* ");
}
int key_insert(key_struct * k){
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
    key_len = readn(key,key_len);
    key_struct *k = key_init(key,key_len);
    if(key_insert(k)){
        puts("[-] Fail to Gen a New Key");
        key_destructor(k);
    }
    else{
        puts("[+] Key Gen Done");
    }
}
void key_menu(){
    puts(" =========================");
    puts("0. Add a Key");
    puts("1. Del a Key");
    puts("2. Show all Keys");
    puts("3. Return");
    puts(" ========================= ");
    printf("> ");
}
void key_management(){
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
    printf("The length of your plaintext: ");
    size_t p_len = readint();
    if(!p_len)
        return ;
    if(p_len>0x1000)
        panic("Too long to keep it secure"); //This is actually a hint!
    uint8_t *plaintext  = (uint8_t *)secure_malloc(p_len);
    printf("Plaintext: ");
    p_len = readn(plaintext,p_len);
    if(p_len<=0x21)
        return ;
    enc(plaintext,p_len,KList[idx]);
    puts("\n[+] Encode Done");
}
void decode(){
    key_list();
    printf("Which key do you want to use: ");
    size_t idx = readint();
    if(idx >= 0x10)
        panic("Invalid data");
    if(KList[idx]==0 || KList[idx]->rb==0 || KList[idx]->rb->pos==0)
    {
        puts("Invalid data");
        return ; 
    }
    printf("The length of your ciphertext: ");
    size_t c_len = readint();
    if(!c_len)
        return; 
    if(c_len>0x2000)
        panic("Invalid data");
    uint8_t *c = secure_malloc(c_len+1);
    printf("Ciphertext: ");
    c_len = readn(c,c_len);
    c[c_len] = 0 ; 

    dec(c,c_len,KList[idx]);
    free(c);
    puts("\n[+] Decode Done");
}
int singleR(){
    int     rnd     = secure_open("/dev/urandom");
    uint8_t orecal  = urand_byte(rnd)%4;
    uint8_t key_len = random_uint8(rnd,1,0x18);
    uint8_t * key   = secure_malloc(key_len+1);
    for(int i =0 ; i<key_len; i++)
        key[i] = urand_byte(rnd) ;
    key[key_len] = 0 ; 

    key_struct *k = key_init(key,key_len);
    enc(challenge_plaintext[orecal],L,k);
    puts("\nWhich one is the plaintext:");
    size_t ans = readint();
    if(ans == orecal){
        return 1;
    }
    return 0;
}
void challge(){
    for(int i =0 ; i < 0x4 ; i++)
    {
        printf("Plaintext %d :\n",i);
        puts(challenge_plaintext[i]);
        puts("");
    }
    size_t win = 0 ; 
    for(int i = 0 ; i < XRound ; i++)
    {
        printf("Round %d: \n",i);
        if(singleR())
            win++;
    }
    if(win != XRound)
        panic("");
    else{
    puts("I am pretty sure you are qualified, and can now challenge more difficult versions.");
    puts("New Randomness:");
    scanf("%lf",&R);
    }
}
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
void logo_loader(){
    char buf[0x100] ; 
    memset(buf,0,sizeof(buf));
    int f =  secure_open("./logo");
    size_t res = read(f,buf,sizeof(buf));
    if(res>sizeof(buf))
        panic("Read error");
    puts(buf);
}
void init_challenge_plaintext()
{
    int urd     = secure_open("/dev/urandom");
    FILE * fp   = fopen("./samples", "r");
    if (fp == NULL)
        panic("Fopen error");

    for(int i = 0 ; i<0x4; i++)
    {
        challenge_plaintext[i] = secure_malloc(L+1);
        size_t idx = random_uint8(urd,0,100);
        fseek(fp, idx*(L+1) , SEEK_SET);
        fread(challenge_plaintext[i],L,1,fp);
        challenge_plaintext[i][L] = 0 ;
    }
    close(urd);
    fclose(fp);
    return ;
}
void init_keytable(){
    
}
void init(){
    fclose(stderr);
    setvbuf(stdin,  0, 2, 0);
    setvbuf(stdout, 0, 2, 0);
    init_challenge_plaintext();
    init_keytable();
    logo_loader();
}
int main(){
    alarm(1200); //I'll set the alarm in set-up.sh so I can remove this.
    init();
    play_ground();
}