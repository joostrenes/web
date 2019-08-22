#include "../fe25519.h"

static crypto_uint32 equal(crypto_uint32 a,crypto_uint32 b) /* 16-bit inputs */
{
  crypto_uint32 x = a ^ b; /* 0: yes; 1..65535: no */
  x -= 1; /* 4294967295: yes; 0..65534: no */
  x >>= 31; /* 1: yes; 0: no */
  return x;
}

static crypto_uint32 ge(crypto_uint32 a,crypto_uint32 b) /* 16-bit inputs */
{
  unsigned int x = a;
  x -= (unsigned int) b; /* 0..65535: yes; 4294901761..4294967295: no */
  x >>= 31; /* 0: yes; 1: no */
  x ^= 1; /* 1: yes; 0: no */
  return x;
}

static crypto_uint32 times19(crypto_uint32 a)
{
  return (a << 4) + (a << 1) + a;
}

static crypto_uint32 times38(crypto_uint32 a)
{
  return (a << 5) + (a << 2) + (a << 1);
}

static void reduce_add_sub(fe25519 *r)
{
  crypto_uint32 t;
  int i,rep;

  for(rep=0;rep<4;rep++)
  {
    t = r->b[31] >> 7;
    r->b[31] &= 127;
    t = times19(t);
    r->b[0] += t;
    for(i=0;i<31;i++)
    {
      t = r->b[i] >> 8;
      r->b[i+1] += t;
      r->b[i] &= 255;
    }
  }
}

static void reduce_mul(fe25519 *r)
{
  crypto_uint32 t;
  int i,rep;

  for(rep=0;rep<2;rep++)
  {
    t = r->b[31] >> 7;
    r->b[31] &= 127;
    t = times19(t);
    r->b[0] += t;
    for(i=0;i<31;i++)
    {
      t = r->b[i] >> 8;
      r->b[i+1] += t;
      r->b[i] &= 255;
    }
  }
}

/* reduction modulo 2^255-19 */
void fe25519_freeze(fe25519 *r) 
{
  int i;
    reduce_mul(r); // ADDED -- otherwise got exception
  crypto_uint32 m = equal(r->b[31],127);
  for(i=30;i>0;i--)
    m &= equal(r->b[i],255);
  m &= ge(r->b[0],237);

  m = -m;

  r->b[31] -= m&127;
  for(i=30;i>0;i--)
    r->b[i] -= m&255;
  r->b[0] -= m&237;
}

void fe25519_unpack(fe25519 *r, const unsigned char x[32])
{
  int i;
  for(i=0;i<32;i++) r->b[i] = x[i];
  r->b[31] &= 127;
}

/* Assumes input x being reduced below 2^255 */
void fe25519_pack(unsigned char r[32], const fe25519 *x)
{
  int i;
  fe25519 y = *x;
  fe25519_freeze(&y);
  for(i=0;i<32;i++) 
    r[i] = y.b[i];
}

void fe25519_copy(fe25519 *r, const fe25519 *x)
{
  int i;
    for(i=0;i<32;i++) { r->b[i] = x->b[i]; }
}

int fe25519_iszero(const fe25519 *x)
{
  int i;
  fe25519 t = *x;
  fe25519_freeze(&t);
  int r = equal(t.b[0],0);
  for(i=1;i<32;i++) 
    r &= equal(t.b[i],0);
  return r;
}

void fe25519_setone(fe25519 *r)
{
  int i;
  r->b[0] = 1;
  for(i=1;i<32;i++) r->b[i]=0;
}

void fe25519_setzero(fe25519 *r)
{
  int i;
  for(i=0;i<32;i++) r->b[i]=0;
}

void fe25519_add(fe25519 *r, const fe25519 *x, const fe25519 *y)
{
  int i;
  for(i=0;i<32;i++) r->b[i] = x->b[i] + y->b[i];
  reduce_add_sub(r);
}

void fe25519_sub(fe25519 *r, const fe25519 *x, const fe25519 *y)
{
  int i;
  crypto_uint32 t[32];
  t[0] = x->b[0] + 0x1da;
  t[31] = x->b[31] + 0xfe;
  for(i=1;i<31;i++) t[i] = x->b[i] + 0x1fe;
  for(i=0;i<32;i++) r->b[i] = t[i] - y->b[i];
  reduce_add_sub(r);
}

void fe25519_mulconst(fe25519 *r, const fe25519 *x, uint16 y)
{
    unsigned char c0 = (y & 0xFF);
    unsigned char c1 = ((y >> 8) & 0xFF);

    const fe25519 c = { .b = {c0,c1,0,0,0,0,0,0,
                                0,0,0,0,0,0,0,0,
                                0,0,0,0,0,0,0,0,
                                0,0,0,0,0,0,0,0} };
    fe25519_mul(r, x, &c);
}

void fe25519_mul(fe25519 *r, const fe25519 *x, const fe25519 *y)
{
  int i,j;
  crypto_uint32 t[63];
  for(i=0;i<63;i++)t[i] = 0;

  for(i=0;i<32;i++)
    for(j=0;j<32;j++)
      t[i+j] += x->b[i] * y->b[j];

  for(i=32;i<63;i++)
    r->b[i-32] = t[i-32] + times38(t[i]); 
  r->b[31] = t[31]; /* result now in r[0]...r[31] */

  reduce_mul(r);
}

void fe25519_square(fe25519 *r, const fe25519 *x)
{
  fe25519_mul(r, x, x);
}

void fe25519_invert(fe25519 *r, const fe25519 *x)
{
    fe25519 z2;
    fe25519 t1;
    fe25519 t0;
    fe25519 z9;
    fe25519 z11;
    int i;
    
    /* 2 */ fe25519_square(&z2,x);
    /* 4 */ fe25519_square(&t1,&z2);
    /* 8 */ fe25519_square(&t0,&t1);
    /* 9 */ fe25519_mul(&z9,&t0,x);
    /* 11 */ fe25519_mul(&z11,&z9,&z2);
    /* 22 */ fe25519_square(&t0,&z11);
    /* 2^5 - 2^0 = 31 */ fe25519_mul(&z2,&t0,&z9);

    /* 2^6 - 2^1 */ fe25519_square(&t0,&z2);
    /* 2^7 - 2^2 */ fe25519_square(&t1,&t0);
    /* 2^8 - 2^3 */ fe25519_square(&t0,&t1);
    /* 2^9 - 2^4 */ fe25519_square(&t1,&t0);
    /* 2^10 - 2^5 */ fe25519_square(&t0,&t1);
    /* 2^10 - 2^0 */ fe25519_mul(&z2,&t0,&z2);

    /* 2^11 - 2^1 */ fe25519_square(&t0,&z2);
    /* 2^12 - 2^2 */ fe25519_square(&t1,&t0);
    /* 2^20 - 2^10 */ for (i = 2;i < 10;i += 2) { fe25519_square(&t0,&t1); fe25519_square(&t1,&t0); }
    /* 2^20 - 2^0 */ fe25519_mul(&z9,&t1,&z2);

    /* 2^21 - 2^1 */ fe25519_square(&t0,&z9);
    /* 2^22 - 2^2 */ fe25519_square(&t1,&t0);
    /* 2^40 - 2^20 */ for (i = 2;i < 20;i += 2) { fe25519_square(&t0,&t1); fe25519_square(&t1,&t0); }
    /* 2^40 - 2^0 */ fe25519_mul(&t0,&t1,&z9);

    /* 2^41 - 2^1 */ fe25519_square(&t1,&t0);
    /* 2^42 - 2^2 */ fe25519_square(&t0,&t1);
    /* 2^50 - 2^10 */ for (i = 2;i < 10;i += 2) { fe25519_square(&t1,&t0); fe25519_square(&t0,&t1); }
    /* 2^50 - 2^0 */ fe25519_mul(&z2,&t0,&z2);

    /* 2^51 - 2^1 */ fe25519_square(&t0,&z2);
    /* 2^52 - 2^2 */ fe25519_square(&t1,&t0);
    /* 2^100 - 2^50 */ for (i = 2;i < 50;i += 2) { fe25519_square(&t0,&t1); fe25519_square(&t1,&t0); }
    /* 2^100 - 2^0 */ fe25519_mul(&z9,&t1,&z2);

    /* 2^101 - 2^1 */ fe25519_square(&t1,&z9);
    /* 2^102 - 2^2 */ fe25519_square(&t0,&t1);
    /* 2^200 - 2^100 */ for (i = 2;i < 100;i += 2) { fe25519_square(&t1,&t0); fe25519_square(&t0,&t1); }
    /* 2^200 - 2^0 */ fe25519_mul(&t1,&t0,&z9);

    /* 2^201 - 2^1 */ fe25519_square(&t0,&t1);
    /* 2^202 - 2^2 */ fe25519_square(&t1,&t0);
    /* 2^250 - 2^50 */ for (i = 2;i < 50;i += 2) { fe25519_square(&t0,&t1); fe25519_square(&t1,&t0); }
    /* 2^250 - 2^0 */ fe25519_mul(&t0,&t1,&z2);

    /* 2^251 - 2^1 */ fe25519_square(&t1,&t0);
    /* 2^252 - 2^2 */ fe25519_square(&t0,&t1);
    /* 2^253 - 2^3 */ fe25519_square(&t1,&t0);
    /* 2^254 - 2^4 */ fe25519_square(&t0,&t1);
    /* 2^255 - 2^5 */ fe25519_square(&t1,&t0);
    /* 2^255 - 21 */ fe25519_mul(r,&t1,&z11);
}
