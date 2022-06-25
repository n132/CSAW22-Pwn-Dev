#include<stdio.h>
#include<stdint.h>
#include<fcntl.h>
#include<stdlib.h>
#include<unistd.h>
double R = 0.32;
size_t L = 0x400;
void panic(char *s)
{
    puts(s);
    exit(-1);
}
uint8_t urand_byte(int f)
{
    uint8_t r;
    read(f,&r,1);
    return r;    
}
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
void check(void *p)
{
    if(p<=0)
        panic("fail to allocate more memory");
}
key_struct * key_init(uint8_t *key,size_t kl)
{
    key_struct *k = 0 ;
    k = malloc(sizeof(key_struct));
    check(k);
    k->rb = malloc(sizeof(random_bytes));
    check(k->rb);
    k->rb->pos = 0;
    k->rb->cur = 0;
    k->rb->limit = 0;
    k->key = key;
    k->key_len = kl;
    return k; 
}
void enc(uint8_t *plaintext,uint8_t *key, size_t key_len)
{
    int f       = 0;
    uint8_t * p = 0;
    size_t ct   = 0;
    size_t rct  = 0;
    uint8_t * e = 0;
    
    f   = open("/dev/urandom",0,0);
    p   = plaintext; 
    e   = realloc(0,L);
    if( f<0)
        panic("enc");
    check(e);

    key_struct *k = key_init(key,key_len);
    
    while( p[ct-rct]!= 0)
    {
        if(urand_byte(f)<= R * 0x100)
        {
            if(k->rb->cur >= k->rb->limit)
            {

                k->rb->pos = realloc(k->rb->pos,sizeof(size_t) * (k->rb->limit+0x20));
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
int main()
{
    uint8_t p[]= {1,2,3,4,5,6,67};
    uint8_t k[]= {0};
    enc(p,k,sizeof(k));
}