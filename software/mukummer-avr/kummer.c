#include "kummer.h"
#include "gfe.h"
#include "jac.h"

static void cswap4x(unsigned char *x, unsigned char *y, int b)
{
  unsigned char db = -b;
  unsigned char t;
  unsigned char i;
  for(i=0;i<=63;i++) {
      t = x[i] ^ y[i];
      t &= db;
      x[i] ^= t;
      y[i] ^= t;
    }
}

void crypto_scalarmult(unsigned char *q, const unsigned char *n, const unsigned char *p, const int l)
{
  int i;
  int bit;
  int prevbit;
  int swap;

  // initiate to identity
  q[0] = 11;
  q[16] = 22;
  q[32] = 19;
  q[48] = 3;

  for(i=1; i<=15; i++)
  {
    q[i] = 0;
    q[i+16] = 0;
    q[i+32] = 0;
    q[i+48] = 0;
  }

  xUNWRAP(q+64, p);

  prevbit = 0;
  for(i=l;i>=0;i--) 
  {
      bit = (n[i>>3]>>(i&0x07)) & 1;
      swap = bit ^ prevbit;
      prevbit = bit;

      gfe_neg(q+64, q+64);
      cswap4x(q,q+64,swap);
      xDBLADD(q, q+64, q, q+64, p);
  }
  gfe_neg(q, q);
  cswap4x(q,q+64,bit);
}

// assumes that z[0] and t[0] are negated
// the output w0[0] is negated
// if w0 != z,t and w4 != t,z, then z and t are unchanged
void xDBLADD(unsigned char *w0, unsigned char *w4, const unsigned char *z, const unsigned char *t, const unsigned char *p)
{
  hadamard(w4, t);
  hadamard(w0, z);
  
  gfe_mul(w4,w4,w0);
  gfe_mul(w4+16,w4+16,w0+16);
  gfe_mul(w4+32,w4+32,w0+32);
  gfe_mul(w4+48,w4+48,w0+48);
 
  gfe_square(w0,w0);
  gfe_square(w0+16,w0+16);
  gfe_square(w0+32,w0+32);
  gfe_square(w0+48,w0+48);

  gfe_mulconst(w4,w4,BBCCDD);
  gfe_mulconst(w4+16,w4+16,AACCDD);
  gfe_mulconst(w4+32,w4+32,AABBDD);
  gfe_mulconst(w4+48,w4+48,AABBCC);

  gfe_mulconst(w0,w0,BBCCDD);
  gfe_mulconst(w0+16,w0+16,AACCDD);
  gfe_mulconst(w0+32,w0+32,AABBDD);
  gfe_mulconst(w0+48,w0+48,AABBCC);

  hadamard(w4, w4);
  hadamard(w0, w0);

  gfe_square(w4,w4);
  gfe_square(w4+16,w4+16);
  gfe_square(w4+32,w4+32);
  gfe_square(w4+48,w4+48);
  
  gfe_square(w0,w0);
  gfe_square(w0+16,w0+16);
  gfe_square(w0+32,w0+32);
  gfe_square(w0+48,w0+48);

  gfe_mul(w4+16,w4+16, p);
  gfe_mul(w4+32,w4+32, p+16);
  gfe_mul(w4+48,w4+48, p+32);

  gfe_mulconst(w0,w0,bbccdd);
  gfe_mulconst(w0+16,w0+16,aaccdd);
  gfe_mulconst(w0+32,w0+32,aabbdd);
  gfe_mulconst(w0+48,w0+48,aabbcc);
}
