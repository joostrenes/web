#include "gfe.h"
#include "jac.h"
#include "kummer.h"

void copy(unsigned char *r, const unsigned char *x, unsigned long long xlen)
{
    unsigned int i;

    for(i=0; i<=xlen-1; i++)
    {
        r[i] = x[i];
    }
}

// project a point on the Jacobian to a point on the Kummer
void project(unsigned char *r, const unsigned char *p)
{
    gfe work[8]; 

    gfe_sub(work[0], mu, p+16);
    gfe_sub(work[1], nulambda, p+16);
    gfe_sub(work[2], nu, p+16);
    gfe_sub(work[3], mulambda, p+16);

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

    gfe_mul(work[4], work[4], p+16);
    gfe_mul(work[5], work[5], p+16);
    gfe_mul(work[6], work[6], p+16);
    gfe_mul(work[7], work[7], p+16);

    gfe_square(work[0], p+48);
    gfe_sub(work[4], work[4], work[0]);
    gfe_sub(work[5], work[5], work[0]);
    gfe_sub(work[6], work[6], work[0]);
    gfe_sub(work[7], work[7], work[0]);

    gfe_mulconst(work[4], work[4], acoeff);
    gfe_neg(r, work[4]);
    gfe_mulconst(r+16, work[5], bcoeff);
    gfe_mulconst(r+32, work[6], ccoeff);
    gfe_mulconst(r+48, work[7], dcoeff);
}

void jacobian_scalarmult(unsigned char *r, const unsigned char *n, const unsigned char *p, const unsigned char *xpw)
{
    unsigned char xp[64], xqxpq[128];
    int len = 250;

    xUNWRAP(xp, xpw);
    crypto_scalarmult(xqxpq, n, xpw, len);
    recoverFast(r, p, xp, xqxpq, xqxpq+64);

    gfe_pack(r, r);
    gfe_pack(r+16, r+16);
    gfe_pack(r+32, r+32);
    gfe_pack(r+48, r+48);
}

void twodim_scalarmult(unsigned char *r, const unsigned char *Pdec, const unsigned char *xpw, const unsigned char *Qdec, const unsigned char *m, const unsigned char *n)
{
    unsigned char nq[64], mp[64];
    unsigned char xqxpq[128];
    int len;

    len = 127;
    project(mp, Qdec);
    xWRAP(r, mp);
    crypto_scalarmult(xqxpq, n, r, len);
    recoverFast(nq, Qdec, mp, xqxpq, xqxpq+64);

    len = 250;
    xUNWRAP(r, xpw);
    crypto_scalarmult(xqxpq, m, xpw, len);
    recoverFast(mp, Pdec, r, xqxpq, xqxpq+64);

    ADD(r, nq, mp);
    
    gfe_pack(r, r);
    gfe_pack(r+16, r+16);
    gfe_pack(r+32, r+32);
    gfe_pack(r+48, r+48);
}

void ADD(unsigned char *r, const unsigned char *x, const unsigned char *y)
{
    gfe work[9];

    // map to the curve with coeff of x^4 zero
    gfe_add(work[0], alphadiv5, alphadiv5); // 2a/5     
    gfe_sub(work[1], x, work[0]); // q1 - 2a/5
    gfe_sub(work[8], y, work[0]); // q2 - 2a/5
    gfe_add(work[3], work[1], alphadiv5); // q1 + a/5
    gfe_mul(work[3], work[3], alphadiv5); // a/5*(q1 + a/5)
    gfe_sub(work[3], x+16, work[3]); // r1 - a/5*(q1 + a/5);
    gfe_add(work[4], work[8], alphadiv5); // q2 + a/5
    gfe_mul(work[4], work[4], alphadiv5); // a/5*(q2 + a/5)
    gfe_sub(work[4], y+16, work[4]); // r2 - a/5*(q2 + a/5);
    gfe_mul(work[5], x+32, alphadiv5); // a/5*s1
    gfe_sub(work[5], x+48, work[5]); // t1 - a/5*s1
    gfe_mul(work[6], y+32, alphadiv5); // a/5*s2
    gfe_sub(work[6], y+48, work[6]); // t2 - a/5*s2

    // now treat coordinates as on the new curve!
    gfe_sub(r, work[1], work[8]); // q1-q2
    gfe_sub(r+16, work[3], work[4]); // r1-r2

    gfe_mul(work[2], work[8], r); // q2(q1-q2)
    gfe_sub(work[2], work[2], r+16); // q2(q1-q2)-(r1-r2)

    gfe_sub(r+32, work[5], work[6]); // t1-t2
    gfe_mul(r+48, r, r+32); // (q1-q2)*(t1-t2)
    gfe_mul(r+32, r+32, work[2]); // (t1-t2)*[q2(q1-q2)-(r1-r2)]
    gfe_mul(work[2], work[2], r+16); // (r1-r2)*[q2(q1-q2)-(r1-r2)]

    gfe_mul(work[4], r, work[4]); // r2(q1-q2)
    gfe_sub(work[7], x+32, y+32); // s1-s2
    gfe_mul(r+16, r+16, work[7]); // (r1-r2)(s1-s2)
    gfe_sub(r+48, r+48, r+16); // C

    gfe_mul(work[7], work[7], work[4]); // r2(q1-q2)(s1-s2)
    gfe_sub(r+32, r+32, work[7]); // A
    gfe_mul(work[4], work[4], r); // r2(q1-q2)^2
    gfe_sub(r+16, work[2], work[4]); // B

    // (r[0], r[1], r[2], r[3]) = (q1-q2, B, A, C)
    gfe_mul(work[7], r+16, r+48); // BC
    gfe_invert(work[4], work[7]); // 1/BC
    gfe_mul(work[7], work[4], r+16); // 1/C
    gfe_mul(work[6], work[7], r+16); // B/C
    gfe_mul(r+32, work[7], r+32); // A/C

    gfe_add(work[7], r, r+32); // q1-q2+A/C
    gfe_add(r, work[7], r+32); // q1-q2+2A/C
    gfe_mul(r+16, work[7], r+32); // A/C(q1-q2+A/C)

    gfe_square(work[7], work[6]); // B^2/C^2
    gfe_sub(r, r, work[7]); // q3

    gfe_add(work[8], work[1], work[8]); // q1+q2
    gfe_mul(work[8], work[8], work[6]); // B/C(q1+q2)
    gfe_sub(work[8], work[8], x+32); // B/C(q1+q2)-s1
    gfe_sub(work[8], work[8], y+32); // B/C(q1+q2)-s1-s2
    gfe_mul(work[8], work[8], work[6]); // B/C[B/C(q1+q2)-s1-s2]

    gfe_add(r+16, r+16, work[8]); // r3

    // (r[0], r[1], r[2], r[3], work[4]) = (q3, r3, A/C, C, 1/BC)
    gfe_square(r+48, r+48); // C^2 
    gfe_mul(r+48, r+48, work[4]); // C/B

    gfe_sub(work[1], work[1], r); // q1-q3
    gfe_sub(work[3], work[3], r+16); // r1-r3

    gfe_mul(work[4], work[3], r+32); // A/C*(r1-r3)
    gfe_mul(work[2], work[1], r+16); // r3*(q1-q3)
    gfe_sub(work[4], work[4], work[2]); // A/C*(r1-r3)-r3*(q1-q3)

    gfe_mul(work[2], work[1], r); // q3*(q1-q3)
    gfe_sub(work[3], work[3], work[2]); // r1-r3-q3*(q1-q3)
    gfe_mul(work[2], work[1], r+32); // A/C*(q1-q3)
    gfe_add(work[3], work[3], work[2]); // r1-r3-q3*(q1-q3)+A/C*(q1-q3)

    gfe_mul(r+32, work[3], r+48); // C/B*[r1-r3-q3*(q1-q3)+A/C*(q1-q3)]
    gfe_mul(r+48, work[4], r+48); // C/B*[A/C*(r1-r3)-r3*(q1-q3)]

    gfe_sub(r+32, r+32, x+32); // s3
    gfe_sub(r+48, r+48, work[5]); // t3

    // transform back to original curve
    gfe_add(r, r, work[0]); // q3 done
    gfe_sub(work[0], r, alphadiv5);
    gfe_mul(work[0], work[0], alphadiv5);
    gfe_add(r+16, r+16, work[0]); // r3 done
    gfe_mul(work[0], r+32, alphadiv5); // alpha/5*s3
    gfe_add(r+48, r+48, work[0]); // t3 done
}

void compress(unsigned char *r, const unsigned char *x)
{
    gfe_mul(r, x, x+48); // a1b0        
    gfe_mul(r+16, x+16, x+32); // a0b1
    gfe_sub(r, r, r+16); // a1b0-a0b1
    gfe_mul(r, r, x+32); // (a1b0-a0b1)b1
    gfe_square(r+16, x+48); // b0^2
    gfe_sub(r, r, r+16); // (a1b0-a0b1)b1-b0^2
    gfe_add(r, r, r); 
    gfe_add(r, r, r); // *4
    gfe_pack(r, r);

    r[15] = x[15] ^ (r[0] << 7); // set top byte of a1
    copy(r, x, 15); // copy the rest

    r[31] = x[31] ^ (x[32] << 7); // set top byte of a0
    copy(r+16, x+16, 15); // copy the rest
}

void decompress(unsigned char *r, const unsigned char *x)
{
    char bit1, bit0;
    gfe work[4];

    // take bits from top bytes
    copy(r, x, 16);
    copy(r+16, x+16, 16);
    bit1 = (r[15] & 0x80) >> 7;
    bit0 = (r[31] & 0x80) >> 7;
    r[15] &= 0x7f;
    r[31] &= 0x7f;

    // a1 in r[0], a0 in r[1]
    gfe_square(r+32, r); // a1^2
    gfe_sub(work[2], r+16, r+32); // a0-a1^2
    gfe_add(work[3], r+16, work[2]); // 2a0-a1^2
    gfe_mul(r+48, work[3], f4); // f4(2a0-a1^2)

    gfe_mul(work[0], f3, r); // f3a1
    gfe_sub(work[0], work[0], f2); // f3a1-f2
    gfe_sub(work[0], work[0], f2); // f3a1-2f2

    gfe_add(r+48, r+48, work[0]); // f4(2a0-a1^2)+(f3a1-2f2)
    gfe_mul(r+48, r+48, r+16); // a0(f4(2a0-a1^2)+(f3a1-2f2))
     
    gfe_neg(work[3], work[3]); // a1^2-2a0
    gfe_sub(r+32, work[3], r+16); // a1^2-3a0
    gfe_mul(work[0], r+32, r+16); // a0(a1^2-3a0)
    gfe_add(work[0], work[0], f1); // f1+a0(a1^2-3a0)
    gfe_mul(work[0], work[0], r); // a1(f1+a0(a1^2-3a0))

    gfe_add(r+48, r+48, work[0]);
    gfe_add(r+48, r+48, r+48); // B
    
    gfe_sub(r+32, r+32, r+16); // A = a1^2-4a0

    gfe_sub(work[0], r, f4); // a1-f4
    gfe_mul(work[0], work[0], r); // a1(a1-f4)
    gfe_add(work[0], f3, work[0]); // f3+a1(a1-f4)
    gfe_sub(work[0], r+16, work[0]); // a0-f3-a1(a1-f4)
    gfe_mul(work[0], work[0], r+16); // a0(a0-f3-a1(a1-f4))
    gfe_add(work[0], work[0], f1); // a0(a0-f3-a1(a1-f4))+f1
    gfe_square(work[0], work[0]); // C
    
    // a1 in r[0], a0 in r[1]
    // A in r[2], B in r[3], C in work[0]
    // a0-a1^2 in work[2], a1^2-2a0 in work[3]
    gfe_add(r+32, r+32, r+32); // 2A
    gfe_add(work[0], work[0], work[0]); // 2C
    gfe_mul(work[0], r+32, work[0]); // 4AC
    gfe_square(work[1], r+48); // B^2
    gfe_sub(work[0], work[1], work[0]); // B^2-4AC

    gfe_sqrtinv(work[1],work[0],work[0],r+32, bit1);
    gfe_sub(work[0], work[0], r+48); // z0 - B
    gfe_mul(r+48, work[0], work[1]); // z1 = (z0 - B)/(2A) 

    // a1 in r[0], a0 in r[1]
    // z1 in r[3]
    // a0-a1^2 in work[2], a1^2-2a0 in work[3]
    gfe_add(work[0], work[3], f3); // f3+a1^2-2a0
    gfe_mul(work[0], work[0], r); // a1(f3+a1^2-2a0)
    gfe_neg(work[1], work[2]); // a1^2-a0
    gfe_mul(work[1], work[1], f4); // f4(a1^2-a0)
    gfe_sub(work[0], work[1], work[0]); // f4(a1^2-a0)-a1(f3+a1^2-2a0)
    gfe_add(work[0], work[0], f2);
    gfe_add(work[0], work[0], r+48);

    gfe_add(work[1], work[0], work[0]);
    gfe_add(work[1], work[1], work[1]); // 4b1^2
    gfe_powminhalf(work[1], work[1]); // 1/(2b1)
    gfe_add(r+32, work[1], work[1]); // 1/b1
    gfe_mul(r+32, r+32, work[0]); // b1
    gfe_pack(r+32, r+32);
    bit0 ^= r[32];
    if ( bit0 & 1 )
    {
        gfe_neg(work[1], work[1]); // +-1/(2b1)
        gfe_neg(r+32, r+32); // +-b1
    }

    // a1 in r[0], a0 in r[1], b1 in r[2]
    // z1 in r[3], 1/(2b1) in work[0]
    // a0-a1^2 in work[2]
    gfe_sub(work[2], work[2], f3); // a0-a1^2-f3
    gfe_mul(work[0], f4, r); // f4a1
    gfe_add(work[0], work[0], work[2]); // f4a1-f3-a1^2+a0
    gfe_mul(work[0], work[0], r+16); // a0(f4a1-f3-a1^2+a0)
    
    gfe_mul(r+48, r, r+48); // a1z1
    gfe_add(r+48, r+48, work[0]); 
    gfe_add(r+48, r+48, f1);
    gfe_mul(r+48, r+48, work[1]); // b0
    
    gfe_pack(r+32, r+32);
    gfe_pack(r+48, r+48);
}

// assumes that x[0] and y[0] are negated
void xADD(unsigned char *r, const unsigned char *x, const unsigned char *y, const unsigned char *p)
{
  unsigned char work[64];

  hadamard(work, x);
  hadamard(r, y);

  gfe_mul(r, r, work);
  gfe_mul(r+16, r+16, work+16);
  gfe_mul(r+32, r+32, work+32);
  gfe_mul(r+48, r+48, work+48);

  gfe_mulconst(r,r,BBCCDD);
  gfe_mulconst(r+16,r+16,AACCDD);
  gfe_mulconst(r+32,r+32,AABBDD);
  gfe_mulconst(r+48,r+48,AABBCC);

  hadamard(r, r);

  gfe_square(r, r);
  gfe_square(r+16, r+16);
  gfe_square(r+32, r+32);
  gfe_square(r+48, r+48);

  gfe_mul(r, r, p+16); // xY
  gfe_mul(r+16, r+16, p); // yX
  gfe_mul(r+32, r+32, p+48); // zT
  gfe_mul(r+48, r+48, p+32); // tZ

  gfe_mul(work, p, p+16); // XY
  gfe_mul(work+16, p+32, p+48); // ZT

  gfe_mul(r, r, work+16);
  gfe_mul(r+16, r+16, work+16);
  gfe_mul(r+32, r+32, work);
  gfe_mul(r+48, r+48, work);
}

void xWRAP(unsigned char *r, const unsigned char *p)
{
  gfe work[4];

  gfe_mul(work[0], p+16, p+32); // YZ
  gfe_mul(work[1], work[0], p+48); // YZT
  gfe_invert(work[2], work[1]); // 1/YZT
  gfe_mul(work[2], work[2], p); // X/YZT
  gfe_mul(work[3], work[2], p+48); // X/YZ
  gfe_mul(r, work[3], p+32); // X/Y
  gfe_mul(r+16, work[3], p+16); // X/Z
  gfe_mul(r+32, work[0], work[2]); // X/T
}

void xUNWRAP(unsigned char *r, const unsigned char *p)
{
  gfe_mul(r+48, p, p+16); // X^2/YZ
  gfe_mul(r+32, p, p+32); // X^2/YT 
  gfe_mul(r+16, p+16, p+32); // X^2/ZT
  gfe_mul(r, r+48, p+32);  // X^3/YZT
}

void fast2genPartial(unsigned char *r, const unsigned char *p)
{
    gfe temp;    

    gfe_mul(r, M01, p+16); 
    gfe_add(r, p, r);
    gfe_mul(r+16, M02, p+32);
    gfe_add(r, r+16, r);
    gfe_mul(r+16, M03, p+48);
    gfe_add(r, r+16, r);

    gfe_mul(r+16, M10, p);
    gfe_mul(r+32, M11, p+16);
    gfe_add(r+16, r+32, r+16);
    gfe_mul(r+32, M12, p+32);
    gfe_add(r+16, r+32, r+16);
    gfe_mul(r+32, M13, p+48);
    gfe_add(r+16, r+32, r+16);

    gfe_mul(r+32, M20, p);
    gfe_mul(temp, M21, p+16);
    gfe_add(r+32, temp, r+32);
    gfe_mul(temp, M22, p+32);
    gfe_add(r+32, temp, r+32);
    gfe_mul(temp, M23, p+48);
    gfe_add(r+32, temp, r+32);
}

void fast2genFull(unsigned char *r, const unsigned char *p)
{
    gfe temp;    

    fast2genPartial(r, p);

    gfe_mul(r+48, M30, p);
    gfe_mul(temp, M31, p+16);
    gfe_add(r+48, temp, r+48);
    gfe_mul(temp, M32, p+32);
    gfe_add(r+48, temp, r+48);
    gfe_mul(temp, M33, p+48);
    gfe_add(r+48, temp, r+48);
}

// r = result
// p = Jacobian point P = (a1, a0, b1, b0)
// xp = Fast Kummer point x(P)
// xqxpq = [x(Q), x(Q+P)]
void recoverFast(unsigned char *r, const unsigned char *p, const unsigned char *xp, const unsigned char *xq, const unsigned char *xpq)
{
    unsigned char xpGen[48], xqpGen[48];
    unsigned char temp[128];

    copy(temp, xq, 64);
    copy(temp+64, xp, 64);

    // compute x(Q-P)
    gfe_neg(temp+64, temp+64);
    gfe_neg(temp, temp);
    xADD(r, temp+64, temp, xpq);

    // map to general Kummer
    fast2genPartial(xpGen, xp);
    fast2genFull(temp, xq);
    fast2genPartial(xqpGen, r);
    fast2genPartial(temp+64, xpq);

    recoverGeneral(r, p, xpGen, temp, xqpGen, temp+64);
}

// r = result
// p = Jacobian point P = (a1, a0, b1, b0)
// xp = General Kummer point x(P) (3 coords)
// xq = General Kummer point x(Q) (4 coords)
// xqp = General Kummer point x(Q-P) = x(P-Q) (3 coords)
// xpq = General Kummer point x(Q+P) (3 coords)
void recoverGeneral(unsigned char *r, const unsigned char *p, const unsigned char *xp, const unsigned char *xq, const unsigned char *xqp, const unsigned char *xpq)
{
    gfe work[9];

    // 1 -- all temps free
    // work[2] = Z1
    gfe_mul(work[2], xp+16, xq); 
    gfe_mul(work[0], xp, xq+16);
    gfe_sub(work[2], work[2], work[0]);

    // 2 -- work[2,3] in use
    // work[3] = Z2
    gfe_mul(work[3], xp, xq+32);
    gfe_mul(work[0], xp+32, xq);
    gfe_sub(work[3], work[3], work[0]);

    // 3 -- work[2,3,4,5,6,7] in use
    // work[4] = Z1P3
    // work[5] = mZ3
    // work[6] = D
    // work[7] = E 
    gfe_mul(work[4], work[2], xp+32);
    gfe_mul(work[5], work[3], xp+16);
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
    gfe_mul(work[0], xq+48, work[6]);
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

    gfe_mul(work[1], work[5], xq+16);
    gfe_mul(work[0], work[3], xq+32);
    gfe_mul(work[0], work[0], xp);
    gfe_sub(work[1], work[1], work[0]);

    gfe_mul(work[1], work[1], work[5]); // delete mZ3
    gfe_mul(work[1], work[1], xq);
    gfe_add(work[7], work[7], work[1]);

    // 5 -- work[2,3,4,5,6,7,8,9] in use
    // work[5] = X1
    gfe_mul(work[5], work[3], p+32);
    gfe_mul(work[0], work[2], p+48);
    gfe_sub(work[5], work[5], work[0]);
    gfe_mul(work[5], work[5], xp);

    // 6 -- work[2,3,4,5,6,7,8,9,10] in use
    // r+16 = X2
    gfe_mul(work[1], work[2], xp+16);
    gfe_mul(work[0], work[3], xp);
    gfe_add(work[1], work[1], work[0]);

    gfe_mul(r+16, work[4], p+32);
    gfe_mul(work[0], work[1], p+48);
    gfe_add(r+16, r+16, work[0]);

    // 7 -- work[1,2,3,4,5,6,7,8,9,10] in use 
    // r+32 = C5
    gfe_square(r+32, work[2]); 
    gfe_mul(work[0], work[1], xq);
    gfe_sub(r+32, r+32, work[0]);

    // 8 -- work[2,3,4,5,6,7,8,9,10] in use
    // work[4] = C6
    gfe_mul(work[4], work[4], xq); // delete Z1P3
    gfe_add(work[4], work[4], work[8]); // delete Z1Z2

    // 9 -- work[2,3,4,5,6,7,8,9,10] in use
    // xqp = [sum1, sum2, sum3]
    // xpq = [dif1, dif2, dif3]
    // work[8] = t13
    // r+48 = X3
    gfe_mul(work[8], xqp+32, xpq);
    gfe_mul(work[0], xpq+32, xqp);
    gfe_sub(work[8], work[8], work[0]);

    gfe_mul(work[1], xpq+16, xqp);
    gfe_mul(work[0], xpq, xqp+16);
    gfe_sub(work[1], work[1], work[0]);

    gfe_mul(work[1], work[1], r+16);
    gfe_mul(work[0], work[8], work[5]);
    gfe_sub(r+48, work[0], work[1]);

    // 10 -- work[2,3,4,5,6,7,8,9,10] in use
    // r+32 = X5
    gfe_mul(r+32, r+32, r+48); // delete C5

    // 11 -- work[2,3,4,5,6,7,8,9,10] in use
    // work[4] = X6
    gfe_mul(work[4], work[4], r+48); // deleteX C6, X3

    // 12 -- work[2,3,4,6,7,8,9,10] in use
    // work[8] = X4
    gfe_mul(work[1], xpq+16, xqp+32);
    gfe_mul(work[0], xqp+16, xpq+32);
    gfe_sub(work[1], work[1], work[0]);

    gfe_mul(work[1], work[1], work[5]);
    gfe_mul(work[0], work[8], r+16); // delete t13
    gfe_add(work[1], work[1], work[0]);
    gfe_mul(work[8], work[1], r);

    // 13 -- work[2,3,4,5,6,7,8] in use
    // work[2] = X7
    gfe_mul(work[0], work[2], work[8]); // delete Z1
    gfe_add(work[2], r+32, work[0]); // delete X5

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
    gfe_mul(work[1], xpq+16, xqp);
    gfe_mul(work[0], xqp+16, xpq);
    gfe_add(work[1], work[1], work[0]);

    gfe_mul(work[2], xqp+32, xpq);
    gfe_mul(work[0], xpq+32, xqp);
    gfe_add(work[2], work[2], work[0]);

    gfe_mul(work[1], work[1], r+16);
    gfe_mul(work[2], work[2], work[5]);
    gfe_add(work[1], work[1], work[2]); 

    gfe_mul(work[1], work[1], work[5]); // delete X1
    gfe_square(work[0], r+16); // delete X2
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

    gfe_mul(r, xq+16, work[5]);
    gfe_neg(r, r);
    gfe_mul(r+16, xq+32, work[5]); 
    gfe_mul(work[5], work[5], xq);
    gfe_invert(work[4], work[5]); // delete F

    gfe_mul(r, r, work[4]);
    gfe_mul(r+16, r+16, work[4]);
    gfe_mul(r+32, work[8], work[4]); // delete X9
    gfe_mul(r+48, work[3], work[4]); // delete X10

    gfe_pack(r, r);
    gfe_pack(r+16, r+16);
    gfe_pack(r+32, r+32);
    gfe_pack(r+48, r+48);
}
