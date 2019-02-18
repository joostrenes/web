#include "gfe.h"
#include "jac.h"
#include "kummer.h"

void copy(uint32 *r, const uint32 *x, unsigned long long xlen)
{
    unsigned int i;

    for(i=0; i<=xlen-1; i++)
    {
        r[i] = x[i];
    }
}

// project a point on the Jacobian to a point on the Kummer
void project(uint32 *r, const uint32 *p)
{
    gfe work[8]; 

    gfe_sub(work[0], mu, p+4);
    gfe_sub(work[1], nulambda, p+4);
    gfe_sub(work[2], nu, p+4);
    gfe_sub(work[3], mulambda, p+4);

    gfe_add(work[4], lambda, p);
    gfe_add(work[6], work[4], mu);
    gfe_add(work[4], work[4], nu);
    
    gfe_mul(work[4], work[4], work[0]);
    gfe_mul(work[6], work[6], work[2]);

    gfe_add(work[5], mu, p);
    gfe_mul(work[5], work[5], work[1]);
    gfe_add(work[5], work[5], work[1]);
    gfe_add(work[7], nu, p);
    gfe_mul(work[7], work[7], work[3]);
    gfe_add(work[7], work[7], work[3]);

    gfe_mul(work[4], work[4], p+4);
    gfe_mul(work[5], work[5], p+4);
    gfe_mul(work[6], work[6], p+4);
    gfe_mul(work[7], work[7], p+4);

    gfe_square(work[0], p+12);
    gfe_sub(work[4], work[4], work[0]);
    gfe_sub(work[5], work[5], work[0]);
    gfe_sub(work[6], work[6], work[0]);
    gfe_sub(work[7], work[7], work[0]);

    gfe_mulconst(work[4], work[4], acoeff);
    gfe_neg(r, work[4]);
    gfe_mulconst(r+4, work[5], bcoeff);
    gfe_mulconst(r+8, work[6], ccoeff);
    gfe_mulconst(r+12, work[7], dcoeff);
}

void jacobian_scalarmult(uint32 *r, const uint32 *n, const uint32 *p, const uint32 *xpw)
{
    uint32 xp[16], xqxpq[32];
    int l = 250;

    xUNWRAP(xp, xpw);
    crypto_scalarmult(xqxpq, n, xpw, l);
    recoverFast(r, p, xp, xqxpq, xqxpq+16);

    gfe_pack(r, r);
    gfe_pack(r+4, r+4);
    gfe_pack(r+8, r+8);
    gfe_pack(r+12, r+12);
}

void twodim_scalarmult(uint32 *r, const uint32 *Pdec, const uint32 *xpw, const uint32 *Qdec, const uint32 *m, const uint32 *n)
{
    uint32 mp[16], nq[16];
    uint32 xqxpq[32];
    int len;

    len = 127;
    project(mp, Qdec);
    xWRAP(r, mp);
    crypto_scalarmult(xqxpq, n, r, len);
    recoverFast(nq, Qdec, mp, xqxpq, xqxpq+16);

    len = 250;
    xUNWRAP(r, xpw);
    crypto_scalarmult(xqxpq, m, xpw, len);
    recoverFast(mp, Pdec, r, xqxpq, xqxpq+16);
    
    ADD(r, nq, mp);

    gfe_pack(r, r);
    gfe_pack(r+4, r+4);
    gfe_pack(r+8, r+8);
    gfe_pack(r+12, r+12);
}

/*
void twodim_scalarmult(uint32 *r, const uint32 *Pdec, const uint32 *xpw, const uint32 *Qdec, const uint32 *m, const uint32 *n)
{
    uint32 xqw[12];
    uint32 mp[16], nq[16];

    project(nq, Qdec);
    xWRAP(xqw, nq);
    jacobian_scalarmult128(nq, n, Qdec, xqw);
    jacobian_scalarmult(mp, m, Pdec, xpw);
    ADD(r, mp, nq);
}
*/

void ADD(uint32 *r, const uint32 *x, const uint32 *y)
{
    gfe work[9];

    // map to the curve with coeff of x^4 zero
    gfe_add(work[0], alphadiv5, alphadiv5); // 2a/5     
    gfe_sub(work[1], x, work[0]); // q1 - 2a/5
    gfe_sub(work[8], y, work[0]); // q2 - 2a/5
    gfe_add(work[3], work[1], alphadiv5); // q1 + a/5
    gfe_mul(work[3], work[3], alphadiv5); // a/5*(q1 + a/5)
    gfe_sub(work[3], x+4, work[3]); // r1 - a/5*(q1 + a/5);
    gfe_add(work[4], work[8], alphadiv5); // q2 + a/5
    gfe_mul(work[4], work[4], alphadiv5); // a/5*(q2 + a/5)
    gfe_sub(work[4], y+4, work[4]); // r2 - a/5*(q2 + a/5);
    gfe_mul(work[5], x+8, alphadiv5); // a/5*s1
    gfe_sub(work[5], x+12, work[5]); // t1 - a/5*s1
    gfe_mul(work[6], y+8, alphadiv5); // a/5*s2
    gfe_sub(work[6], y+12, work[6]); // t2 - a/5*s2

    // now treat coordinates as on the new curve!

    gfe_sub(r, work[1], work[8]); // q1-q2
    gfe_sub(r+4, work[3], work[4]); // r1-r2

    gfe_mul(work[2], work[8], r); // q2(q1-q2)
    gfe_sub(work[2], work[2], r+4); // q2(q1-q2)-(r1-r2)

    gfe_sub(r+8, work[5], work[6]); // t1-t2
    gfe_mul(r+12, r, r+8); // (q1-q2)*(t1-t2)
    gfe_mul(r+8, r+8, work[2]); // (t1-t2)*[q2(q1-q2)-(r1-r2)]
    gfe_mul(work[2], work[2], r+4); // (r1-r2)*[q2(q1-q2)-(r1-r2)]

    gfe_mul(work[4], r, work[4]); // r2(q1-q2)
    gfe_sub(work[7], x+8, y+8); // s1-s2
    gfe_mul(r+4, r+4, work[7]); // (r1-r2)(s1-s2)
    gfe_sub(r+12, r+12, r+4); // C

    gfe_mul(work[7], work[7], work[4]); // r2(q1-q2)(s1-s2)
    gfe_sub(r+8, r+8, work[7]); // A
    gfe_mul(work[4], work[4], r); // r2(q1-q2)^2
    gfe_sub(r+4, work[2], work[4]); // B

    // (r[0], r[1], r[2], r[3]) = (q1-q2, B, A, C)

    gfe_mul(work[7], r+4, r+12); // BC
    gfe_invert(work[4], work[7]); // 1/BC
    gfe_mul(work[7], work[4], r+4); // 1/C
    gfe_mul(work[6], work[7], r+4); // B/C
    gfe_mul(r+8, work[7], r+8); // A/C

    gfe_add(work[7], r, r+8); // q1-q2+A/C
    gfe_add(r, work[7], r+8); // q1-q2+2A/C
    gfe_mul(r+4, work[7], r+8); // A/C(q1-q2+A/C)

    gfe_square(work[7], work[6]); // B^2/C^2
    gfe_sub(r, r, work[7]); // q3

    gfe_add(work[8], work[1], work[8]); // q1+q2
    gfe_mul(work[8], work[8], work[6]); // B/C(q1+q2)
    gfe_sub(work[8], work[8], x+8); // B/C(q1+q2)-s1
    gfe_sub(work[8], work[8], y+8); // B/C(q1+q2)-s1-s2
    gfe_mul(work[8], work[8], work[6]); // B/C[B/C(q1+q2)-s1-s2]

    gfe_add(r+4, r+4, work[8]); // r3

    // (r[0], r[1], r[2], r[3], work[4]) = (q3, r3, A/C, C, 1/BC)

    gfe_square(r+12, r+12); // C^2 
    gfe_mul(r+12, r+12, work[4]); // C/B

    gfe_sub(work[1], work[1], r); // q1-q3
    gfe_sub(work[3], work[3], r+4); // r1-r3

    gfe_mul(work[4], work[3], r+8); // A/C*(r1-r3)
    gfe_mul(work[2], work[1], r+4); // r3*(q1-q3)
    gfe_sub(work[4], work[4], work[2]); // A/C*(r1-r3)-r3*(q1-q3)

    gfe_mul(work[2], work[1], r); // q3*(q1-q3)
    gfe_sub(work[3], work[3], work[2]); // r1-r3-q3*(q1-q3)
    gfe_mul(work[2], work[1], r+8); // A/C*(q1-q3)
    gfe_add(work[3], work[3], work[2]); // r1-r3-q3*(q1-q3)+A/C*(q1-q3)

    gfe_mul(r+8, work[3], r+12); // C/B*[r1-r3-q3*(q1-q3)+A/C*(q1-q3)]
    gfe_mul(r+12, work[4], r+12); // C/B*[A/C*(r1-r3)-r3*(q1-q3)]

    gfe_sub(r+8, r+8, x+8); // s3
    gfe_sub(r+12, r+12, work[5]); // t3

    // transform back to original curve
    gfe_add(r, r, work[0]); // q3 done
    gfe_sub(work[0], r, alphadiv5);
    gfe_mul(work[0], work[0], alphadiv5);
    gfe_add(r+4, r+4, work[0]); // r3 done
    gfe_mul(work[0], r+8, alphadiv5); // alpha/5*s3
    gfe_add(r+12, r+12, work[0]); // t3 done
}

void compress(uint32 *r, const uint32 *x)
{
    gfe_mul(r, x, x+12); // a1b0        
    gfe_mul(r+4, x+4, x+8); // a0b1
    gfe_sub(r, r, r+4); // a1b0-a0b1
    gfe_mul(r, r, x+8); // (a1b0-a0b1)b1
    gfe_square(r+4, x+12); // b0^2
    gfe_sub(r, r, r+4); // (a1b0-a0b1)b1-b0^2
    gfe_add(r, r, r); 
    gfe_add(r, r, r); // *4
    gfe_pack(r, r);

    r[3] = x[3] ^ (r[0] << 31);
    copy(r, x, 3);

    r[7] = x[7] ^ (x[8] << 31);
    copy(r+4, x+4, 3);
}

void decompress(uint32 *r, const uint32 *x)
{
    char bit1, bit0;
    gfe work[4];

    copy(r, x, 4);
    copy(r+4, x+4, 4);
    bit1 = (unsigned char)((r[3] & 0x80000000) >> 31);
    bit0 = (unsigned char)((r[7] & 0x80000000) >> 31);
    r[3] &= 0x7fffffff;
    r[7] &= 0x7fffffff;

    // a1 in r[0], a0 in r[1]
    gfe_square(r+8, r); // a1^2
    gfe_sub(work[2], r+4, r+8); // a0-a1^2
    gfe_add(work[3], r+4, work[2]); // 2a0-a1^2
    gfe_mul(r+12, work[3], f4); // f4(2a0-a1^2)

    gfe_mul(work[0], f3, r); // f3a1
    gfe_sub(work[0], work[0], f2); // f3a1-f2
    gfe_sub(work[0], work[0], f2); // f3a1-2f2

    gfe_add(r+12, r+12, work[0]); // f4(2a0-a1^2)+(f3a1-2f2)
    gfe_mul(r+12, r+12, r+4); // a0(f4(2a0-a1^2)+(f3a1-2f2))
     
    gfe_neg(work[3], work[3]); // a1^2-2a0
    gfe_sub(r+8, work[3], r+4); // a1^2-3a0
    gfe_mul(work[0], r+8, r+4); // a0(a1^2-3a0)
    gfe_add(work[0], work[0], f1); // f1+a0(a1^2-3a0)
    gfe_mul(work[0], work[0], r); // a1(f1+a0(a1^2-3a0))

    gfe_add(r+12, r+12, work[0]);
    gfe_add(r+12, r+12, r+12); // B
    
    gfe_sub(r+8, r+8, r+4); // A = a1^2-4a0

    gfe_sub(work[0], r, f4); // a1-f4
    gfe_mul(work[0], work[0], r); // a1(a1-f4)
    gfe_add(work[0], f3, work[0]); // f3+a1(a1-f4)
    gfe_sub(work[0], r+4, work[0]); // a0-f3-a1(a1-f4)
    gfe_mul(work[0], work[0], r+4); // a0(a0-f3-a1(a1-f4))
    gfe_add(work[0], work[0], f1); // a0(a0-f3-a1(a1-f4))+f1
    gfe_square(work[0], work[0]); // C
    
    // a1 in r[0], a0 in r[1]
    // A in r[2], B in r[3], C in work[0]
    // a0-a1^2 in work[2], a1^2-2a0 in work[3]
    gfe_add(r+8, r+8, r+8); // 2A
    gfe_add(work[0], work[0], work[0]); // 2C
    gfe_mul(work[0], r+8, work[0]); // 4AC
    gfe_square(work[1], r+12); // B^2
    gfe_sub(work[0], work[1], work[0]); // B^2-4AC

    gfe_sqrtinv(work[1],work[0],work[0],r+8, bit1);
    gfe_sub(work[0], work[0], r+12); // z0 - B
    gfe_mul(r+12, work[0], work[1]); // z1 = (z0 - B)/(2A) 

    // a1 in r[0], a0 in r[1]
    // z1 in r[3]
    // a0-a1^2 in work[2], a1^2-2a0 in work[3]
    gfe_add(work[0], work[3], f3); // f3+a1^2-2a0
    gfe_mul(work[0], work[0], r); // a1(f3+a1^2-2a0)
    gfe_neg(work[1], work[2]); // a1^2-a0
    gfe_mul(work[1], work[1], f4); // f4(a1^2-a0)
    gfe_sub(work[0], work[1], work[0]); // f4(a1^2-a0)-a1(f3+a1^2-2a0)
    gfe_add(work[0], work[0], f2);
    gfe_add(work[0], work[0], r+12);

    gfe_add(work[1], work[0], work[0]);
    gfe_add(work[1], work[1], work[1]); // 4b1^2
    gfe_powminhalf(work[1], work[1]); // 1/(2b1)
    gfe_add(r+8, work[1], work[1]); // 1/b1
    gfe_mul(r+8, r+8, work[0]); // b1
    gfe_pack(r+8, r+8);
    bit0 ^= r[8];
    if ( bit0 & 1 )
    {
        gfe_neg(work[1], work[1]); // +-1/(2b1)
        gfe_neg(r+8, r+8); // +-b1
    }

    // a1 in r[0], a0 in r[1], b1 in r[2]
    // z1 in r[3], 1/(2b1) in work[0]
    // a0-a1^2 in work[2]
    gfe_sub(work[2], work[2], f3); // a0-a1^2-f3
    gfe_mul(work[0], f4, r); // f4a1
    gfe_add(work[0], work[0], work[2]); // f4a1-f3-a1^2+a0
    gfe_mul(work[0], work[0], r+4); // a0(f4a1-f3-a1^2+a0)
    
    gfe_mul(r+12, r, r+12); // a1z1
    gfe_add(r+12, r+12, work[0]); 
    gfe_add(r+12, r+12, f1);
    gfe_mul(r+12, r+12, work[1]); // b0

    gfe_pack(r+8, r+8);
    gfe_pack(r+12, r+12);
}

void xUNWRAP(uint32 *r, const uint32 *p)
{
  gfe_mul(r+12, p, p+4); // X^2/YZ
  gfe_mul(r+8, p, p+8); // X^2/YT 
  gfe_mul(r+4, p+4, p+8); // X^2/ZT
  gfe_mul(r, r+12, p+8);  // X^3/YZT
}

void xWRAP(uint32 *r, const uint32 *p)
{
  gfe work[4];

  gfe_mul(work[0], p+4, p+8); // YZ
  gfe_mul(work[1], work[0], p+12); // YZT
  gfe_invert(work[2], work[1]); // 1/YZT
  gfe_mul(work[2], work[2], p); // X/YZT
  gfe_mul(work[3], work[2], p+12); // X/YZ
  gfe_mul(r, work[3], p+8); // X/Y
  gfe_mul(r+4, work[3], p+4); // X/Z
  gfe_mul(r+8, work[0], work[2]); // X/T
}

void fast2genPartial(uint32 *r, const uint32 *p)
{
    gfe temp;    

    gfe_mul(r, M01, p+4); 
    gfe_add(r, p, r);
    gfe_mul(r+4, M02, p+8);
    gfe_add(r, r+4, r);
    gfe_mul(r+4, M03, p+12);
    gfe_add(r, r+4, r);

    gfe_mul(r+4, M10, p);
    gfe_mul(r+8, M11, p+4);
    gfe_add(r+4, r+8, r+4);
    gfe_mul(r+8, M12, p+8);
    gfe_add(r+4, r+8, r+4);
    gfe_mul(r+8, M13, p+12);
    gfe_add(r+4, r+8, r+4);

    gfe_mul(r+8, M20, p);
    gfe_mul(temp, M21, p+4);
    gfe_add(r+8, temp, r+8);
    gfe_mul(temp, M22, p+8);
    gfe_add(r+8, temp, r+8);
    gfe_mul(temp, M23, p+12);
    gfe_add(r+8, temp, r+8);
}

void fast2genFull(uint32 *r, const uint32 *p)
{
    gfe temp;    

    fast2genPartial(r, p);

    gfe_mul(r+12, M30, p);
    gfe_mul(temp, M31, p+4);
    gfe_add(r+12, temp, r+12);
    gfe_mul(temp, M32, p+8);
    gfe_add(r+12, temp, r+12);
    gfe_mul(temp, M33, p+12);
    gfe_add(r+12, temp, r+12);
}

/*
void xADD_start(uint32 *r, const uint32 *x, const uint32 *y)
{
  uint32 work[16];

  hadamard(work, x);
  hadamard(r, y);

  gfe_mul(r, r, work);
  gfe_mul(r+4, r+4, work+4);
  gfe_mul(r+8, r+8, work+8);
  gfe_mul(r+12, r+12, work+12);

  gfe_mulconst(r,r,BBCCDD);
  gfe_mulconst(r+4,r+4,AACCDD);
  gfe_mulconst(r+8,r+8,AABBDD);
  gfe_mulconst(r+12,r+12,AABBCC);

  hadamard(r, r);

  gfe_square(r, r);
  gfe_square(r+4, r+4);
  gfe_square(r+8, r+8);
  gfe_square(r+12, r+12);
}
*/
/*
// assumes that x[0] and y[0] are negated
void xADD_nodiv(uint32 *r, const uint32 *x, const uint32 *y, const uint32 *p)
{
  uint32 work[8];

  xADD_start(r, x, y);

  gfe_mul(r, r, p+4); // xY
  gfe_mul(r+4, r+4, p); // yX
  gfe_mul(r+8, r+8, p+12); // zT
  gfe_mul(r+12, r+12, p+8); // tZ

  gfe_mul(work, p, p+4); // XY
  gfe_mul(work+4, p+8, p+12); // ZT

  gfe_mul(r, r, work+4);
  gfe_mul(r+4, r+4, work+4);
  gfe_mul(r+8, r+8, work);
  gfe_mul(r+12, r+12, work);
}
*/

// assumes that x[0] and y[0] are negated
void xADD(uint32 *r, const uint32 *x, const uint32 *y, const uint32 *p)
{
  uint32 work[16];

  hadamard(work, x);
  hadamard(r, y);

  gfe_mul(r, r, work);
  gfe_mul(r+4, r+4, work+4);
  gfe_mul(r+8, r+8, work+8);
  gfe_mul(r+12, r+12, work+12);

  gfe_mulconst(r,r,BBCCDD);
  gfe_mulconst(r+4,r+4,AACCDD);
  gfe_mulconst(r+8,r+8,AABBDD);
  gfe_mulconst(r+12,r+12,AABBCC);

  hadamard(r, r);

  gfe_square(r, r);
  gfe_square(r+4, r+4);
  gfe_square(r+8, r+8);
  gfe_square(r+12, r+12);

  gfe_mul(r, r, p+4); // xY
  gfe_mul(r+4, r+4, p); // yX
  gfe_mul(r+8, r+8, p+12); // zT
  gfe_mul(r+12, r+12, p+8); // tZ

  gfe_mul(work, p, p+4); // XY
  gfe_mul(work+4, p+8, p+12); // ZT

  gfe_mul(r, r, work+4);
  gfe_mul(r+4, r+4, work+4);
  gfe_mul(r+8, r+8, work);
  gfe_mul(r+12, r+12, work);
}

// r = result
// p = Jacobian point P = (a1, a0, b1, b0)
// xp = Fast Kummer point x(P)
// xqxpq = [x(Q), x(Q+P)]
void recoverFast(uint32 *r, const uint32 *p, const uint32 *xp, const uint32 *xq, const uint32 *xpq)
{
    uint32 xpGen[12], xqpGen[12];
    uint32 temp[32];

    copy(temp, xq, 16);
    copy(temp+16, xp, 16);

    // compute x(Q-P)
    gfe_neg(temp+16, temp+16);
    gfe_neg(temp, temp);
    xADD(r, temp+16, temp, xpq);

    // map to general Kummer
    fast2genPartial(xpGen, xp);
    fast2genFull(temp, xq);
    fast2genPartial(xqpGen, r);
    fast2genPartial(temp+16, xpq);

    recoverGeneral(r, p, xpGen, temp, xqpGen, temp+16);
}

// r = result
// p = Jacobian point P = (a1, a0, b1, b0)
// xp = General Kummer point x(P) (3 coords)
// xq = General Kummer point x(Q) (4 coords)
// xqp = General Kummer point x(Q-P) = x(P-Q) (3 coords)
// xpq = General Kummer point x(Q+P) (3 coords)
void recoverGeneral(uint32 *r, const uint32 *p, const uint32 *xp, const uint32 *xq, const uint32 *xqp, const uint32 *xpq)
{
    gfe work[9];

    // 1 -- all temps free
    // work[2] = Z1
    gfe_mul(work[2], xp+4, xq); 
    gfe_mul(work[0], xp, xq+4);
    gfe_sub(work[2], work[2], work[0]);

    // 2 -- work[2,3] in use
    // work[3] = Z2
    gfe_mul(work[3], xp, xq+8);
    gfe_mul(work[0], xp+8, xq);
    gfe_sub(work[3], work[3], work[0]);

    // 3 -- work[2,3,4,5,6,7] in use
    // work[4] = Z1P3
    // work[5] = mZ3
    // work[6] = D
    // work[7] = E 
    gfe_mul(work[4], work[2], xp+8);
    gfe_mul(work[5], work[3], xp+4);
    gfe_add(work[5], work[5], work[4]);

    gfe_mul(work[0], work[2], work[5]);
    gfe_square(work[6], work[3]);
    gfe_mul(work[7], work[6], f2); // add this to save a square
    gfe_mul(work[6], work[6], xp);
    gfe_add(work[6], work[6], work[0]);

    // 4 -- work[2,3,4,5,6,7,8,9] in use
    // work[8] = Z1Z2
    // r = P1Q1
    gfe_mul(work[8], work[2], work[3]);
    gfe_mul(r, xp, xq);
    //gfe_square(work[7], work[3]); // do in step 3
    //gfe_mul(work[7], work[7], f2);
    gfe_mul(work[0], f1, work[8]);
    gfe_sub(work[7], work[7], work[0]);

    gfe_mul(work[7], work[7], r);
    gfe_mul(work[0], xq+12, work[6]);
    gfe_add(work[7], work[7], work[0]);

    gfe_mul(work[7], work[7], r);
    gfe_mul(work[1], work[3], xp);
    gfe_mul(work[1], work[1], f3);
    gfe_mul(work[0], f4, work[5]);
    gfe_add(work[1], work[1], work[0]);

    gfe_square(work[0], xq);
    gfe_mul(work[1], work[1], work[0]);
    
    gfe_mul(work[1], work[1], work[5]);
    gfe_add(work[7], work[7], work[1]);

    gfe_mul(work[1], work[5], xq+4);
    gfe_mul(work[0], work[3], xq+8);
    gfe_mul(work[0], work[0], xp);
    gfe_sub(work[1], work[1], work[0]);

    gfe_mul(work[1], work[1], work[5]); // delete mZ3
    gfe_mul(work[1], work[1], xq);
    gfe_add(work[7], work[7], work[1]);

    // 5 -- work[2,3,4,5,6,7,8,9] in use
    // work[5] = X1
    gfe_mul(work[5], work[3], p+8);
    gfe_mul(work[0], work[2], p+12);
    gfe_sub(work[5], work[5], work[0]);
    gfe_mul(work[5], work[5], xp);

    // 6 -- work[2,3,4,5,6,7,8,9,10] in use
    // r+4 = X2
    gfe_mul(work[1], work[2], xp+4);
    gfe_mul(work[0], work[3], xp);
    gfe_add(work[1], work[1], work[0]);

    gfe_mul(r+4, work[4], p+8);
    gfe_mul(work[0], work[1], p+12);
    gfe_add(r+4, r+4, work[0]);

    // 7 -- work[1,2,3,4,5,6,7,8,9,10] in use 
    // r+8 = C5
    gfe_square(r+8, work[2]); 
    gfe_mul(work[0], work[1], xq);
    gfe_sub(r+8, r+8, work[0]);

    // 8 -- work[2,3,4,5,6,7,8,9,10] in use
    // work[4] = C6
    gfe_mul(work[4], work[4], xq); // delete Z1P3
    gfe_add(work[4], work[4], work[8]); // delete Z1Z2

    // 9 -- work[2,3,4,5,6,7,8,9,10] in use
    // xqp = [sum1, sum2, sum3]
    // xpq = [dif1, dif2, dif3]
    // work[8] = t13
    // r+12 = X3
    gfe_mul(work[8], xqp+8, xpq);
    gfe_mul(work[0], xpq+8, xqp);
    gfe_sub(work[8], work[8], work[0]);

    gfe_mul(work[1], xpq+4, xqp);
    gfe_mul(work[0], xpq, xqp+4);
    gfe_sub(work[1], work[1], work[0]);

    gfe_mul(work[1], work[1], r+4);
    gfe_mul(work[0], work[8], work[5]);
    gfe_sub(r+12, work[0], work[1]);

    // 10 -- work[2,3,4,5,6,7,8,9,10] in use
    // r+8 = X5
    gfe_mul(r+8, r+8, r+12); // delete C5

    // 11 -- work[2,3,4,5,6,7,8,9,10] in use
    // work[4] = X6
    gfe_mul(work[4], work[4], r+12); // deleteX C6, X3

    // 12 -- work[2,3,4,6,7,8,9,10] in use
    // work[8] = X4
    gfe_mul(work[1], xpq+4, xqp+8);
    gfe_mul(work[0], xqp+4, xpq+8);
    gfe_sub(work[1], work[1], work[0]);

    gfe_mul(work[1], work[1], work[5]);
    gfe_mul(work[0], work[8], r+4); // delete t13
    gfe_add(work[1], work[1], work[0]);
    gfe_mul(work[8], work[1], r);

    // 13 -- work[2,3,4,5,6,7,8] in use
    // work[2] = X7
    gfe_mul(work[0], work[2], work[8]); // delete Z1
    gfe_add(work[2], r+8, work[0]); // delete X5

    // 14 -- work[2,3,4,5,6,7,8,9,10] in use
    // work[3] = X8
    gfe_mul(work[0], work[3], work[8]); // delete Z2, X4
    gfe_add(work[3], work[4], work[0]); // delete X6

    // 15 -- work[2,3,4,5,6,7,9,10] in use
    // work[4] = sd1
    gfe_mul(work[4], xqp, xpq);
    gfe_mul(work[1], work[5], r);
    gfe_square(work[1], work[1]);

    gfe_square(work[0], xp);
    gfe_mul(work[7], work[7], work[0]);
    gfe_add(work[7], work[7], work[1]);
    gfe_neg(work[7], work[7]);
    gfe_mul(work[7], work[7], work[4]);
    gfe_mul(work[7], work[7], r);

    // 16 -- work[2,3,4,5,6,7,8,9,10] in use
    // work[8] = X9
    gfe_mul(work[8], work[7], work[2]); // delete X7

    // 17 -- work[3,4,5,6,7,8,9,10] in use
    // work[3] = X10
    gfe_mul(work[3], work[7], work[3]); // delete E, X8

    // 18 -- work[0,1,2,3,4,5,6,8,9,10] in use
    gfe_mul(work[1], xpq+4, xqp);
    gfe_mul(work[0], xqp+4, xpq);
    gfe_add(work[1], work[1], work[0]);

    gfe_mul(work[2], xqp+8, xpq);
    gfe_mul(work[0], xpq+8, xqp);
    gfe_add(work[2], work[2], work[0]);

    gfe_mul(work[1], work[1], r+4);
    gfe_mul(work[2], work[2], work[5]);
    gfe_add(work[1], work[1], work[2]); 

    gfe_mul(work[1], work[1], work[5]); // delete X1
    gfe_square(work[0], r+4); // delete X2
    gfe_mul(work[0], work[0], work[4]);
    gfe_add(work[0], work[0], work[0]);

    // work[5] = F
    gfe_add(work[5], work[1], work[0]);
    gfe_add(work[5], work[5], work[5]);
    gfe_mul(work[5], work[5], work[6]); // delete D
    gfe_neg(work[5], work[5]);

    gfe_mul(work[5], work[5], r);
    gfe_square(r, r);
    gfe_mul(work[5], work[5], r); // delete P1Q1
    gfe_mul(work[5], work[5], work[4]); // delete sd1
    gfe_mul(work[5], work[5], xp);

    // 19 -- work[3] in use

    gfe_mul(r, xq+4, work[5]);
    gfe_neg(r, r);
    gfe_mul(r+4, xq+8, work[5]); 
    gfe_mul(work[5], work[5], xq);
    gfe_invert(work[4], work[5]); // delete F

    gfe_mul(r, r, work[4]);
    gfe_mul(r+4, r+4, work[4]);
    gfe_mul(r+8, work[8], work[4]); // delete X9
    gfe_mul(r+12, work[3], work[4]); // delete X10

    gfe_pack(r, r);
    gfe_pack(r+4, r+4);
    gfe_pack(r+8, r+8);
    gfe_pack(r+12, r+12);
}
