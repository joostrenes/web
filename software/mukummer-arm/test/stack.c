#include <stdlib.h>
#include "../signatures.h"
#include "../keyexchange.h"
#include "print.h"
#include "fail.h"
#include "randombytes.h"
#include "../jac.h"
#include "../kummer.h"

#define MAXSTACK 5000

unsigned int ctr;
unsigned char canary;
volatile unsigned char *p;
extern unsigned char _end; 

const union data256 privkey;
union data512 res;
union data256 Q;
union data512 dd;
union data384 sig;
unsigned long long meslen = 128;
unsigned char message[128];
unsigned char resultbit;

uint32 r[32], scal0[8], xpw2[12], xpw1[12];
uint32 jac_p[16];

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

#define WRITE_CANARY(X) {p=X;while(p>= (X-MAXSTACK)) *(p--) = canary;}
 
int main(void)
{
  volatile unsigned char a; /* Mark the beginning of the stack */

  //randombytes(&canary,1);
  canary = 42;

  WRITE_CANARY(&a);
  keygen(&Q, &dd, &privkey);
  ctr = MAXSTACK - stack_count(canary,&a);
  print_stack("keygen",-1,ctr);

  WRITE_CANARY(&a);
  sign(&sig, &Q, message, meslen, &dd);
  ctr = MAXSTACK - stack_count(canary,&a);
  print_stack("sign",-1,ctr);

  WRITE_CANARY(&a);
  verify(&Q, message, meslen, &sig);
  ctr = MAXSTACK - stack_count(canary,&a);
  print_stack("verify",-1,ctr);

/*
  WRITE_CANARY(&a);
  dh_keygen(scal0, scal0);
  ctr = MAXSTACK - stack_count(canary,&a);
  print_stack("dh_keygen",-1,ctr);
  */

  WRITE_CANARY(&a);
  dh_exchange(xpw2, xpw1, scal0);
  ctr = MAXSTACK - stack_count(canary,&a);
  print_stack("dh_exchange",-1,ctr);

  WRITE_CANARY(&a);
  crypto_scalarmult(r, scal0, xpw2, 0xff);
  ctr = MAXSTACK - stack_count(canary,&a);
  print_stack("crypto_scalarmult",-1,ctr);

  WRITE_CANARY(&a);
  jacobian_scalarmult(dd.u32, scal0, dd.u32, xpw2);
  ctr = MAXSTACK - stack_count(canary,&a);
  print_stack("jacobian_scalarmult",-1,ctr);

/*
  WRITE_CANARY(&a);
  jacobian_twodim_mult(res.u32, sig.u32, xpw2, sig.u32, scal0, scal0);
  ctr = MAXSTACK - stack_count(canary,&a);
  print_stack("jacobian_twodim_mult",-1,ctr);
  */

  WRITE_CANARY(&a);
  twodim_scalarmult(res.u32, sig.u32, xpw2, sig.u32, scal0, scal0);
  ctr = MAXSTACK - stack_count(canary,&a);
  print_stack("twodim_scalarmult",-1,ctr);

  write_byte(4);
  while(1);
}
