#include <stdio.h>
#include <stm32f0xx.h>
#include "../../sign.h"
#include "print.h"

unsigned char sk[64];
unsigned char pk[32];
unsigned long long mlen = 1;
unsigned char m[1];
unsigned char sm[65];
unsigned long long smlen;
int ch;

ecp R, P;
fe25519 xpw;
fe25519 r,x,y;
group_scalar n;

int main(void)
{
  char out[500];

  SysTick_Config(12000000);
  SysTick->VAL = 0;
  int ini, fin;

    /* ecswap */

  SysTick_Config(12000000);
  SysTick->VAL = 0;
  ini= SysTick->VAL;
    ecswap(&R,&R,1);
  fin= SysTick->VAL;

  fin = ini-fin;

  sprintf(out, "ecswap: %d\n", fin);
  print(out);

    /* fe25519_add */

  SysTick_Config(12000000);
  SysTick->VAL = 0;
  ini= SysTick->VAL;
    fe25519_add(&r,&x,&y);
  fin= SysTick->VAL;

  fin = ini-fin;

  sprintf(out, "fe25519_add: %d\n", fin);
  print(out);

    /* fe25519_mul */

  SysTick_Config(12000000);
  SysTick->VAL = 0;
  ini= SysTick->VAL;
    fe25519_mul(&r,&x,&y);
  fin= SysTick->VAL;

  fin = ini-fin;

  sprintf(out, "fe25519_mul: %d\n", fin);
  print(out);

    /* fe25519_sqr */

  SysTick_Config(12000000);
  SysTick->VAL = 0;
  ini= SysTick->VAL;
    fe25519_square(&r,&x);
  fin= SysTick->VAL;

  fin = ini-fin;

  sprintf(out, "fe25519_square: %d\n", fin);
  print(out);

    /* fe25519_mulconst */

  SysTick_Config(12000000);
  SysTick->VAL = 0;
  ini= SysTick->VAL;
    fe25519_mulconst(&r,&x,0x0001);
  fin= SysTick->VAL;

  fin = ini-fin;

  sprintf(out, "fe25519_mulconst: %d\n", fin);
  print(out);

    /* keypair */

  SysTick_Config(12000000);
  SysTick->VAL = 0;
  ini= SysTick->VAL;
    keypair(pk, sk);
  fin= SysTick->VAL;

  fin = ini-fin;

  sprintf(out, "keypair: %d\n", fin);
  print(out);

    /* sign */

  SysTick_Config(12000000);
  SysTick->VAL = 0;
  ini= SysTick->VAL;
    sign(sm, &smlen, m, mlen, pk, sk);
  fin= SysTick->VAL;

  fin = ini-fin;

  sprintf(out, "sign: %d\n", fin);
  print(out);

    /* verify */

  SysTick_Config(12000000);
  SysTick->VAL = 0;
  ini= SysTick->VAL;
    ch = verify(m, mlen, sm, smlen, pk);
  fin= SysTick->VAL;

  fin = ini-fin;

  sprintf(out, "verify: %d\n", fin);
  print(out);

    /* ladder */

  SysTick_Config(12000000);
  SysTick->VAL = 0;
  ini= SysTick->VAL;
    ladder(&R, &P, &xpw, &n);
  fin= SysTick->VAL;

  fin = ini-fin;

  sprintf(out, "ladder: %d\n", fin);
  print(out);

    /* bValues */

  SysTick_Config(12000000);
  SysTick->VAL = 0;
  ini= SysTick->VAL;
    bValues(&r, &r, &r, &R, &P);
  fin= SysTick->VAL;

  fin = ini-fin;

  sprintf(out, "bValues: %d\n", fin);
  print(out);

    /* compress */

  SysTick_Config(12000000);
  SysTick->VAL = 0;
  ini= SysTick->VAL;
    compress(&r, &R);
  fin= SysTick->VAL;

  fin = ini-fin;

  sprintf(out, "compress: %d\n", fin);
  print(out);

    /* decompress */

  SysTick_Config(12000000);
  SysTick->VAL = 0;
  ini= SysTick->VAL;
    decompress(&R, &r);
  fin= SysTick->VAL;

  fin = ini-fin;

  sprintf(out, "decompress: %d\n", fin);
  print(out);

    print("\n");
    print("\n");
    print("\n");
    print("\n");
    print("\n");
    print("\n");
  
  write_byte(4);

  while(1);
}

void SysTick_Handler(void)
{
  ;
}
