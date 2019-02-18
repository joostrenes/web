#include <stdlib.h>
#include "print.h"
#include "fail.h"
#include "../sign.h"

#define nlen 32
#define qlen 32

#define MAXSTACK 5000

unsigned char i;
unsigned char n[nlen];
unsigned char q[qlen];

unsigned int ctr;
unsigned char canary;
volatile unsigned char *p;
extern unsigned char _end; 

static unsigned int stack_count(unsigned char canary,volatile unsigned char *a)
{
  volatile unsigned char *p = (a-MAXSTACK);
  unsigned int c = 0;
  while(*p == canary && p < a)
  {
    p++;
    c++;
  }
  return c;
} 

unsigned char sk[64];
unsigned char pk[32];
unsigned long long mlen = 1;
unsigned char m[1];
unsigned char sm[65];
unsigned long long smlen = 65;
int ch;

#define WRITE_CANARY(X) {p=X;while(p>= (X-MAXSTACK)) *(p--) = canary;}
 
int main(void)
{
  volatile unsigned char a; /* Mark the beginning of the stack */

  //randombytes(&canary,1);
  canary = 42;

#if 0
  WRITE_CANARY(&a);
    keypair(pk, sk);
  ctr = MAXSTACK - stack_count(canary,&a);
  print_stack("keypair",-1,ctr);
#endif

  WRITE_CANARY(&a);
    sign(sm, &smlen, m, mlen, pk, sk);
  ctr = MAXSTACK - stack_count(canary,&a);
  print_stack("sign",-1,ctr);

#if 0
  WRITE_CANARY(&a);
    ch = verify(m, mlen, sm, smlen, pk);
  ctr = MAXSTACK - stack_count(canary,&a);
  print_stack("verify",-1,ctr);
#endif

  write_byte(4);
  while(1);
}
