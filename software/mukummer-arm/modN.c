#include "gfe.h"
#include "modN.h"
#include "jac.h"

void group_ops(uint32 *s, const uint32 *r, const uint32 *h, const uint32 *d)
{
    uint32 temp512[16];

    bigint_red256(s, h); // s less than 250 bits
    gfe_mul16(s, s);
    bigint_mul256(temp512, s, d);
    gfe_sub512(temp512, r, temp512);
    bigint_red256(s, temp512);
}

void red(uint32 *r, const uint32 *x)
{
    unsigned char i;
    uint32 temp[16];

    copy(temp, x, 8);
    for(i=8; i<=15; i++)
    {
        temp[i] = 0;
    }
    bigint_red256(r, temp);
}

// add two 256 bit integers, with a possible carry as input and output
void bigint_add256(uint32 *r, uint32 *carry, const uint32 *x, const uint32 *y)
{
    unsigned char i;
    uint64 t0, t1;

    for(i=0; i<=7; i++)
    {
        t0 = (uint64)x[i];
        t1 = (uint64)y[i];

        t0 += carry[0];
        t0 += t1;
        carry[0] = (t0 >> 32) & 1;
        r[i] = (uint32)t0;
    }
}

// sub two 256 integers, with a possible carry as input and output
void bigint_sub256(uint32 *r, uint32 *carry, const uint32 *x, const uint32 *y)
{
    unsigned char i;
    uint64 t0, t1;

    for(i=0; i<=7; i++)
    {
        t0 = (uint64)x[i];
        t1 = (uint64)y[i];

        t1 += carry[0];
        t0 -= t1;
        carry[0] = (t0 >> 32) & 1;
        r[i] = (uint32)t0;
    }
}

// conditional mask, depending on the carry
void cmask(uint32 *r, const uint32 *x, uint32 *carry)
{
    unsigned char i;

    carry[0] = -carry[0];

    for(i=0; i<=7; i++)
    {
        r[i] = x[i]&carry[0];
    }
}

// carry a carry bit through
void carry(uint32 *r, const uint32 *x, uint32 *carry)
{
    unsigned char i;
    uint64 t0;

    for(i=0; i<=3; i++)
    {
        t0 = (uint64)x[i];
        t0 += carry[0];
        carry[0] = (t0 >> 32) & 1;
        r[i] = (uint32)t0;
    }
}

void bigint_mul256(uint32 *r, const uint32 *x, const uint32 *y)
{
    uint32 c,k;
    uint32 temp256[8];

    bigint_mul(r, x, y);
    bigint_mul(r+8, x+4, y+4);
    bigint_mul(temp256, x, y+4);

    c = 0;
    bigint_add256(r+4, &c, r+4, temp256);

    k = c;
    bigint_mul(temp256, x+4, y);

    c = 0;
    bigint_add256(r+4, &c, r+4, temp256);

    k += c;

    carry(r+12, r+12, &k);
}

void gfe_sub512(uint32 *r, const uint32 *x, const uint32 *y)
{
    unsigned char i;
    uint32 c;
    uint32 temp256[8];

    c = 0;

    bigint_sub256(r, &c, x, y);
    bigint_sub256(r+8, &c, x+8, y+8);

    // can overflow twice, need to carry
    for(i=0; i<=1; i++)
    {
        cmask(temp256, l6, &c);
        c = 0;
        bigint_sub256(r+8, &c, r+8, temp256);
    }
}

void bigint_red256(uint32 *r, const uint32 *x)
{
    unsigned char i;
    uint32 c;
    uint32 tempmul[16], tempadd[16];

    for(i=0; i<=15; i++)
    {
        tempadd[i] = x[i];
    }

    c = 0;
    // do 4 times to reduce to 260 bits
    for(i=0; i<=3; i++)
    {
        bigint_mul256(tempmul, tempadd+8, l6);
        bigint_add256(tempadd, &c, tempadd, tempmul);

        carry(tempadd+8, tempmul+8, &c);
        carry(tempadd+12, tempmul+12, &c);
    }

    // finalize the last bits    
    // use the top part of tempadd, which is now zero
    tempadd[8] = (tempadd[8]<<6) | ((tempadd[7]&0xfc000000)>>26); 
    tempadd[7] &= 0x03ffffff;

    bigint_mul256(tempmul, tempadd+8, l); 
    bigint_add256(tempadd, &c, tempadd, tempmul);

    // possibly a final reduction
    // set to 0 or 1, depending on bit 250
    // can still be larger than N, but only a little 
    // I.e. bit 250 is zero
    c = (tempadd[7]&0x04000000)>>26;
    tempadd[7] &= 0x03ffffff;

    cmask(tempmul, l, &c);
    c = 0;
    bigint_add256(r, &c, tempadd, tempmul);
}
