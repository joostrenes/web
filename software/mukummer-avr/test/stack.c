#include <stdlib.h>
#include "../signatures.h"
#include "../keyexchange.h"
#include "../Keccak-avr8.h"
#include "../kummer.h"
#include "../jac.h"
#include "print.h"
#include "fail.h"
#include "avr.h"

unsigned int ctr=0,newctr=0;
unsigned char canary;
volatile unsigned char *p;
extern unsigned char _end; 
extern unsigned char __stack; 

static unsigned int stack_count(unsigned char canary)
{
  const unsigned char *p = &_end;
  unsigned int c = 0;
  while(*p == canary && p <= &__stack)
  {
    p++;
    c++;
  }
  return c;
} 

#define WRITE_CANARY(X) {p=X;while(p>= &_end) *(p--) = canary;}

#define keylen 32
#define Qlen 32
#define ddlen 64
#define meslen 32
#define siglen 48
#define scallen 128
//#define checklen 16

static const unsigned char privkey[keylen] = 
                    {0x03,0,0,0,0,0,0,0,
                        0,0,0,0,0,0,0,0,
                        0,0,0,0,0,0,0,0,
                        0,0,0,0,0,0,0,0};
// 1
//static const unsigned char message[meslen] = {0};
// 32
static const unsigned char message[meslen] = 
                    {0,0,0,0,0,0,0,0,
                     0,0,0,0,0,0,0,0,
                     0,0,0,0,0,0,0,0,
                     0,0,0,0,0,0,0,0};
// 64
//static const unsigned char message[meslen] = 
//                    {0,0,0,0,0,0,0,0,
//                     0,0,0,0,0,0,0,0,
//                     0,0,0,0,0,0,0,0,
//                     0,0,0,0,0,0,0,0,
//                     0,0,0,0,0,0,0,0,
//                     0,0,0,0,0,0,0,0,
//                     0,0,0,0,0,0,0,0,
//                     0,0,0,0,0,0,0,0};

#define num 2
 
int main(void)
{
  long long i;
  unsigned long long inlen = meslen;
  volatile unsigned char a; /* Mark the beginning of the stack */

  int l = 250;
  static unsigned char Q[Qlen];
  static unsigned char dd[ddlen];
  static unsigned char sig[siglen];
  //static unsigned char check[checklen];
  static unsigned char scal[scallen];
  static unsigned char hash_out[64];

  for(i=0;i<num-1;i++)
  {
    canary = random();
    WRITE_CANARY(&a);
    crypto_hash(hash_out, message, inlen);
    newctr =(unsigned int)&a - (unsigned int)&_end - stack_count(canary);
    ctr = (newctr>ctr)?newctr:ctr;
  }
  print_stack("hash",-1,ctr);

  ctr = 0;
  newctr = 0;

  for(i=0;i<num-1;i++)
  {
    canary = random();
    WRITE_CANARY(&a);
    keygen(Q, dd, privkey);
    newctr =(unsigned int)&a - (unsigned int)&_end - stack_count(canary);
    ctr = (newctr>ctr)?newctr:ctr;
  }
  print_stack("keygen",-1,ctr);

  ctr = 0;
  newctr = 0;

  for(i=0;i<num-1;i++)
  {
    canary = random();
    WRITE_CANARY(&a);
    sign(sig, Q, message, inlen, dd);
    newctr =(unsigned int)&a - (unsigned int)&_end - stack_count(canary);
    ctr = (newctr>ctr)?newctr:ctr;
  }
  print_stack("sign",-1,ctr);

  ctr = 0;
  newctr = 0;

  for(i=0;i<num-1;i++)
  {
    canary = random();
    WRITE_CANARY(&a);
    verify(Q, message, meslen, sig);
    newctr =(unsigned int)&a - (unsigned int)&_end - stack_count(canary);
    ctr = (newctr>ctr)?newctr:ctr;
  }
  print_stack("verify",-1,ctr);

  ctr = 0;
  newctr = 0;

  for(i=0;i<num-1;i++)
  {
    canary = random();
    WRITE_CANARY(&a);
    dh_exchange(sig, xpw, privkey);
    newctr =(unsigned int)&a - (unsigned int)&_end - stack_count(canary);
    ctr = (newctr>ctr)?newctr:ctr;
  }
  print_stack("dh_exchange",-1,ctr);

  ctr = 0;
  newctr = 0;

  for(i=0;i<num-1;i++)
  {
    canary = random();
    WRITE_CANARY(&a);
    crypto_scalarmult(scal, privkey, sig, l);
    newctr =(unsigned int)&a - (unsigned int)&_end - stack_count(canary);
    ctr = (newctr>ctr)?newctr:ctr;
  }
  print_stack("crypto_scalarmult",-1,ctr);

  for(i=0;i<num-1;i++)
  {
    canary = random();
    WRITE_CANARY(&a);
    jacobian_scalarmult(dd, privkey, dd, sig);
    newctr =(unsigned int)&a - (unsigned int)&_end - stack_count(canary);
    ctr = (newctr>ctr)?newctr:ctr;
  }
  print_stack("jacobian_scalarmult",-1,ctr);

  for(i=0;i<num-1;i++)
  {
    canary = random();
    WRITE_CANARY(&a);
    twodim_scalarmult(dd, dd, sig, dd, privkey, privkey);
    newctr =(unsigned int)&a - (unsigned int)&_end - stack_count(canary);
    ctr = (newctr>ctr)?newctr:ctr;
  }
  print_stack("twodim_scalarmult",-1,ctr);

  avr_end();
  return 0;
}
