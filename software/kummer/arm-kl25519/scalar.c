#include "scalar.h"
#include "test/print.h"

// m = group order..
static const unsigned char m[32] = {0x24,0x8F,0x3B,0x2B,0xA5,0xE1,0x4C,0xFA,
                                    0xD4,0x40,0xA5,0xAA,0xE3,0x83,0x24,0xEB,
                                    0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
                                    0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x7F};

// mu = Floor( 2^512 / m )
static const unsigned char mu[33] = {0x70,0xC3,0x11,0x53,0x6B,0x79,0xCC,0x16,
                                     0xAC,0xFC,0x6A,0x55,0x71,0xF0,0x6D,0x53,
                                     0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
                                     0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x02};

static void bigint_cmov(unsigned char *r, const unsigned char *x, unsigned char b)
{
    unsigned char i;
    unsigned char mask = b;
    mask = -mask;
    for(i=0;i<32;i++)
        r[i] ^= mask & (x[i] ^ r[i]);
}

static void bigint_add(unsigned char *r, const unsigned char *x, const unsigned char *y)
{
    int i;
        uint16 t0, t1;
        unsigned char carry = 0;

    for(i=0; i<32; i++)
    {
        t0 = (uint16)x[i];
        t1 = (uint16)y[i];

        t0 += carry;
                t0 += t1;
        carry = (t0 >> 8) & 1;
        r[i] = (unsigned char)t0;
    }
}

static unsigned char bigint_sub(unsigned char *r, const unsigned char *x, const unsigned char *y)
{
    int i;
        uint16 t0, t1;
        unsigned char carry;

    for(i=0; i<32; i++)
    {
        t0 = (uint16)x[i];
        t1 = (uint16)y[i];

        t1 += carry;
        t0 -= t1;
        carry = (t0 >> 8) & 1;
        r[i] = (unsigned char)t0;
    }

        return carry;
}

static void bigint_mul9(unsigned char *r, const unsigned char *x, const unsigned char *y)
{
    int i,j;
    uint16 t[66];
    uint16 temp;
    uint16 carry;

    for(i=0;i<66;i++) t[i] = 0;
    for(i=0;i<33;i++)
    {
        for(j=0;j<33;j++)
        {
            temp = (uint16)x[i]*(uint16)y[j];

            t[i+j] += temp & 0x00ff;
            t[i+j+1] += (unsigned char)(temp >> 8);
        }
    }

    // carry through
    for(i=0;i<65;i++)
    {
        carry = (t[i] & 0xff00) >> 8;
        t[i] &= 0x00ff;
        t[i+1] += carry;
    }

    for(i=0;i<66;i++)
    {
        r[i] = t[i];
    }
}

static void barrett_reduction(unsigned char *r, unsigned char *a)
{
    unsigned char q1[66], q2[66], n1[33];
    unsigned char c;
    unsigned char i;

    for(i=0;i<32;i++) n1[i] = m[i];
    n1[32] = 0;

    bigint_mul9(q2, a+31, mu);
    bigint_mul9(q1, q2+33, n1);

    bigint_sub(r, a, q1);
    c = bigint_sub(q2, r, m);
    bigint_cmov(r, q2, 1-c);
    c = bigint_sub(q2, r, m);
    bigint_cmov(r, q2, 1-c);
}

void group_scalar_get32(group_scalar *r, const unsigned char x[32])
{
    int i;
    for(i=0;i<32;i++) { r->b[i] = x[i]; }
}

void group_scalar_get64(group_scalar *r, const unsigned char x[64])
{
    int i;
    unsigned char t[64];
    for(i=0;i<64;i++) { t[i] = x[i]; }
    barrett_reduction(r->b, t);
}

void group_scalar_pack(unsigned char r[32], const group_scalar *x)
{
    int i;
    for(i=0;i<32;i++) { r[i] = x->b[i]; }
}

static void group_scalar_setzero(group_scalar *r)
{
    unsigned char i;
    for(i=0;i<32;i++) { r->b[i] = 0; }
}

static void group_scalar_negate(group_scalar *r, const group_scalar *x)
{
    group_scalar t;
    group_scalar_setzero(&t);
    group_scalar_sub(r,&t,x);
}

void group_scalar_set_pos(group_scalar *r)
{
    if ( r->b[0] & 1 ) { group_scalar_negate(r, r); }
}

static void group_scalar_add(group_scalar *r, const group_scalar *x, const group_scalar *y)
{
    unsigned char c;
    unsigned char t[32];
    bigint_add(r->b,x->b,y->b);
    c = bigint_sub(t,r->b,m);
    bigint_cmov(r->b,t,1-c);
}

void group_scalar_sub(group_scalar *r, const group_scalar *x, const group_scalar *y)
{
    unsigned char b = 0;
    uint16_t t;
    unsigned char i;
    group_scalar d;

    for(i=0;i<32;i++)
    {
        t = m[i] - y->b[i] - b;
        d.b[i] = t & 255;
        b = (t >> 8) & 1;
    }
    group_scalar_add(r,x,&d);
}

void group_scalar_mul(group_scalar *r, const group_scalar *x, const group_scalar *y)
{
    union { unsigned char b[64]; uint32 v[16]; } t;
    bigint_mul(t.v, x->v, y->v);
    barrett_reduction(r->b, t.b);
}
