#include "fe25519.h"

/* based on munacl */

void fe25519_freeze(fe25519 *r)
{
    uint32 t0;
    uint32 t1 = (r->v[7]) >> 31;
    uint64 accu;
    uint8 ctr;

    accu = t1 * 19 + 19;
    for (ctr = 0; ctr < 7; ctr++)
    {
        accu += r->v[ctr];
        accu >>= 32;
    }
    accu += r->v[7];

    t0 = (uint32)(accu >> 31);
    accu = t0 * 19;

    for (ctr = 0; ctr < 7; ctr++)
    {
        accu += r->v[ctr];
        r->v[ctr] = (uint32)accu;
        accu >>= 32;
    }
    accu += r->v[7];
    r->v[7] = accu & 0x7ffffffful;
}

void fe25519_unpack(fe25519 *r, const unsigned char x[32])
{
    uint8 ctr;

    for (ctr = 0; ctr < 32; ctr++)
    {
        r->b[ctr] = x[ctr];
    }
    r->b[31] &= 0x7f;
}

void fe25519_pack(unsigned char r[32], const fe25519 *x)
{
    uint8 ctr;
    fe25519 y = *x;

    fe25519_freeze(&y);
    for (ctr = 0; ctr < 32; ctr++)
    {
        r[ctr] = y.b[ctr];
    }
}

void fe25519_copy(fe25519 *r, const fe25519 *x)
{
    uint8 ctr;

    for (ctr = 0; ctr < 8; ctr++)
    {
        r->v[ctr] = x->v[ctr];
    }
}

static uint8 equal(uint32 a, uint32 b)
{
    uint32 x = a ^ b;
    return x == 0;
}

int fe25519_iszero(const fe25519 *x)
{
    uint8 ctr;
    fe25519 t = *x;
    fe25519_freeze(&t);

    int r = equal(t.v[0],0);
    for (ctr = 0; ctr < 8; ctr++)
    {
        r &= equal(t.v[ctr],0);
    }
    return r;
}

void fe25519_setzero(fe25519 *r)
{
    uint8 ctr;

    for (ctr = 0; ctr < 8; ctr++)
    {
        r->v[ctr] = 0;
    }
}

void fe25519_setone(fe25519 *r)
{
    uint8 ctr;

    r->v[0] = 1;
    for (ctr = 1; ctr < 8; ctr++)
    {
        r->v[ctr] = 0;
    }
}

void fe25519_add(fe25519 *r, const fe25519 *x, const fe25519 *y)
{
    uint16 ctr = 0;
    uint64 accu = 0;

    accu = x->v[7];
    accu += y->v[7];
    r->v[7] = ((uint32)accu) & 0x7ffffffful;
    accu = ((uint32)(accu >> 31)) * 19;

    for (ctr = 0; ctr < 7; ctr += 1)
    {
        accu += x->v[ctr];
        accu += y->v[ctr];

        r->v[ctr] = (uint32)accu;
        accu >>= 32;
    }
    accu += r->v[7];
    r->v[7] = (uint32)accu;
}

void fe25519_sub(fe25519 *r, const fe25519 *x, const fe25519 *y)
{
    uint16 ctr = 0;
    int64 accu = 0;

    accu = x->v[7];
    accu -= y->v[7];
    r->v[7] = ((uint32)accu) | 0x80000000ul;
    accu = 19 * ((int32)(accu >> 31) - 1);

    for (ctr = 0; ctr < 7; ctr += 1)
    {
        accu += x->v[ctr];
        accu -= y->v[ctr];

        r->v[ctr] = (uint32)accu;
        accu >>= 32;
    }
    accu += r->v[7];
    r->v[7] = (uint32)accu;
}

void fe25519_mulconst(fe25519 *r, const fe25519 *x, uint16 y)
{
    fe25519_mulconst_asm(r, x, y);
}

void fe25519_mul(fe25519 *r, const fe25519 *x, const fe25519 *y)
{
    uint32 t[16];

    bigint_mul(t, x->v, y->v);
    bigint_red(r, t);
}

void fe25519_square(fe25519 *r, const fe25519 *x)
{
    uint32 t[16];

    bigint_sqr(t, x->v);
    bigint_red(r, t);
}

void fe25519_invert(fe25519 *r, const fe25519 *x)
{
    fe25519 z2;
    fe25519 t0;
    fe25519 z9;
    fe25519 z11;
    int i;
    
    /* 2 */ fe25519_square(&z2,x);
    /* 4 */ fe25519_square(r,&z2);
    /* 8 */ fe25519_square(&t0,r);
    /* 9 */ fe25519_mul(&z9,&t0,x);
    /* 11 */ fe25519_mul(&z11,&z9,&z2);
    /* 22 */ fe25519_square(&t0,&z11);
    /* 2^5 - 2^0 = 31 */ fe25519_mul(&z2,&t0,&z9);

    /* 2^6 - 2^1 */ fe25519_square(&t0,&z2);
    /* 2^7 - 2^2 */ fe25519_square(r,&t0);
    /* 2^8 - 2^3 */ fe25519_square(&t0,r);
    /* 2^9 - 2^4 */ fe25519_square(r,&t0);
    /* 2^10 - 2^5 */ fe25519_square(&t0,r);
    /* 2^10 - 2^0 */ fe25519_mul(&z2,&t0,&z2);

    /* 2^11 - 2^1 */ fe25519_square(&t0,&z2);
    /* 2^12 - 2^2 */ fe25519_square(r,&t0);
    /* 2^20 - 2^10 */ for (i = 2;i < 10;i += 2) { fe25519_square(&t0,r); fe25519_square(r,&t0); }
    /* 2^20 - 2^0 */ fe25519_mul(&z9,r,&z2);

    /* 2^21 - 2^1 */ fe25519_square(&t0,&z9);
    /* 2^22 - 2^2 */ fe25519_square(r,&t0);
    /* 2^40 - 2^20 */ for (i = 2;i < 20;i += 2) { fe25519_square(&t0,r); fe25519_square(r,&t0); }
    /* 2^40 - 2^0 */ fe25519_mul(&t0,r,&z9);

    /* 2^41 - 2^1 */ fe25519_square(r,&t0);
    /* 2^42 - 2^2 */ fe25519_square(&t0,r);
    /* 2^50 - 2^10 */ for (i = 2;i < 10;i += 2) { fe25519_square(r,&t0); fe25519_square(&t0,r); }
    /* 2^50 - 2^0 */ fe25519_mul(&z2,&t0,&z2);

    /* 2^51 - 2^1 */ fe25519_square(&t0,&z2);
    /* 2^52 - 2^2 */ fe25519_square(r,&t0);
    /* 2^100 - 2^50 */ for (i = 2;i < 50;i += 2) { fe25519_square(&t0,r); fe25519_square(r,&t0); }
    /* 2^100 - 2^0 */ fe25519_mul(&z9,r,&z2);

    /* 2^101 - 2^1 */ fe25519_square(r,&z9);
    /* 2^102 - 2^2 */ fe25519_square(&t0,r);
    /* 2^200 - 2^100 */ for (i = 2;i < 100;i += 2) { fe25519_square(r,&t0); fe25519_square(&t0,r); }
    /* 2^200 - 2^0 */ fe25519_mul(r,&t0,&z9);

    /* 2^201 - 2^1 */ fe25519_square(&t0,r);
    /* 2^202 - 2^2 */ fe25519_square(r,&t0);
    /* 2^250 - 2^50 */ for (i = 2;i < 50;i += 2) { fe25519_square(&t0,r); fe25519_square(r,&t0); }
    /* 2^250 - 2^0 */ fe25519_mul(&t0,r,&z2);

    /* 2^251 - 2^1 */ fe25519_square(r,&t0);
    /* 2^252 - 2^2 */ fe25519_square(&t0,r);
    /* 2^253 - 2^3 */ fe25519_square(r,&t0);
    /* 2^254 - 2^4 */ fe25519_square(&t0,r);
    /* 2^255 - 2^5 */ fe25519_square(r,&t0);
    /* 2^255 - 21 */ fe25519_mul(r,r,&z11);
}
