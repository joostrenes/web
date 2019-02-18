#include "kummer.h"
#include "gfe.h"
#include "jac.h"

static void cswap4x(uint32 *x, uint32 *y, int b)
{
  uint32 db = -b;
  uint32 t;
  unsigned char i;
  for(i=0;i<=15;i++) {
      t = x[i] ^ y[i];
      t &= db;
      x[i] ^= t;
      y[i] ^= t;
    }
}

void crypto_scalarmult(uint32 *q, const uint32 *n, const uint32 *p, const int l)
{
  int i;
  int bit;
  int prevbit;
  int swap;

  // initiate to identity
  q[0] = 11;
  q[4] = 22;
  q[8] = 19;
  q[12] = 3;

  for(i=1; i<=3; i++)
  {
    q[i] = 0;
    q[i+4] = 0;
    q[i+8] = 0;
    q[i+12] = 0;
  }

  xUNWRAP(q+16, p);

  prevbit = 0;
  for(i=l;i>=0;i--)
  {
    bit = (n[i>>5]>>(i&0x1f)) & 1;
    swap = bit ^ prevbit;
    prevbit = bit;

    gfe_neg(q+16, q+16);
    cswap4x(q, q+16, swap);
    xDBLADD(q, q+16, q, q+16, p);
  }
  gfe_neg(q, q);
  cswap4x(q,q+16,bit);
}

// assumes that z[0] and t[0] are negated
// the output w0[0] is negated
// if w0 != z,t and w4 != t,z, then z and t are unchanged
void xDBLADD(uint32 *w0, uint32 *w4, const uint32 *z, const uint32 *t, const uint32 *p)
{
  hadamard(w4, t);
  hadamard(w0, z);
  
  gfe_mul(w4,w4,w0);
  gfe_mul(w4+4,w4+4,w0+4);
  gfe_mul(w4+8,w4+8,w0+8);
  gfe_mul(w4+12,w4+12,w0+12);
 
  gfe_square(w0,w0);
  gfe_square(w0+4,w0+4);
  gfe_square(w0+8,w0+8);
  gfe_square(w0+12,w0+12);

  gfe_mulconst(w4,w4,BBCCDD);
  gfe_mulconst(w4+4,w4+4,AACCDD);
  gfe_mulconst(w4+8,w4+8,AABBDD);
  gfe_mulconst(w4+12,w4+12,AABBCC);

  gfe_mulconst(w0,w0,BBCCDD);
  gfe_mulconst(w0+4,w0+4,AACCDD);
  gfe_mulconst(w0+8,w0+8,AABBDD);
  gfe_mulconst(w0+12,w0+12,AABBCC);

  hadamard(w4, w4);
  hadamard(w0, w0);

  gfe_square(w4,w4);
  gfe_square(w4+4,w4+4);
  gfe_square(w4+8,w4+8);
  gfe_square(w4+12,w4+12);
  
  gfe_square(w0,w0);
  gfe_square(w0+4,w0+4);
  gfe_square(w0+8,w0+8);
  gfe_square(w0+12,w0+12);

  gfe_mul(w4+4,w4+4, p);
  gfe_mul(w4+8,w4+8, p+4);
  gfe_mul(w4+12,w4+12, p+8);

  gfe_mulconst(w0,w0,bbccdd);
  gfe_mulconst(w0+4,w0+4,aaccdd);
  gfe_mulconst(w0+8,w0+8,aabbdd);
  gfe_mulconst(w0+12,w0+12,aabbcc);
}
