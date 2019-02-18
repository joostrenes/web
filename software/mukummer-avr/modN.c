#include "gfe.h"
#include "modN.h"

static const unsigned char l6[32] = {0x40, 0x6f, 0x01, 0x03, 0xe1, 0xd2, 0xc2, 0xdc,
                       0xd1, 0x3f, 0x55, 0x68, 0x7e, 0xf2, 0x9f, 0xb0,
                       0x34, 0xa5, 0xd4, 0x31, 0x08, 0xa6, 0x35, 0xcd,
                       0,0,0,0,0,0,0,0};

static const unsigned char l[32] = {0xbd, 0x05, 0x0c, 0x84, 0x4b, 0x0b, 0x73, 0x47,
                       0xff, 0x54, 0xa1, 0xf9, 0xc9, 0x7f, 0xc2, 0xd2,
                       0x94, 0x52, 0xc7, 0x20, 0x98, 0xd6, 0x34, 0x03,
                       0,0,0,0,0,0,0,0};

// add two 256 bit integers, with a possible carry as input and output
void bigint_add256(unsigned char *r, unsigned char *carry, const unsigned char *x, const unsigned char *y)
{
    unsigned char i;
    unsigned short t0, t1;

    for(i=0; i<=31; i++)
    {
        t0 = (unsigned short)x[i];
        t1 = (unsigned short)y[i];

        t0 += carry[0];
        t0 += t1;
        carry[0] = (t0 >> 8) & 1;
        r[i] = (unsigned char)t0;
    }
}

// sub two 256 integers, with a possible carry as input and output
void bigint_sub256(unsigned char *r, unsigned char *carry, const unsigned char *x, const unsigned char *y)
{
    unsigned char i;
    unsigned short t0, t1;

    for(i=0; i<=31; i++)
    {
        t0 = (unsigned short)x[i];
        t1 = (unsigned short)y[i];

        t1 += carry[0];
        t0 -= t1;
        carry[0] = (t0 >> 8) & 1;
        r[i] = (unsigned char)t0;
    }
}

// carry a carry bit through
void carry(unsigned char *r, const unsigned char *x, unsigned char *carry)
{
    unsigned char i;
    unsigned short t0;

    for(i=0; i<=15; i++)
    {
        t0 = (unsigned short)x[i];
        t0 += carry[0];
        carry[0] = (t0 >> 8) & 1;
        r[i] = (unsigned char)t0;
    }
}

// conditional mask, depending on the carry
void cmask(unsigned char *r, const unsigned char *x, unsigned char *carry)
{
    unsigned char i;

    carry[0] = -carry[0];

    for(i=0; i<=31; i++)
    {
        r[i] = x[i]&carry[0];
    }
}

void group_ops(unsigned char *s, const unsigned char *r, const unsigned char *h, const unsigned char *d)
{
    unsigned char temp512[64];

    bigint_red256(s, h); // s less than 250 bits
    gfe_mul16(s, s);
    bigint_mul256(temp512, s, d);
    gfe_sub512(temp512, r, temp512);
    bigint_red256(s, temp512);
}

void gfe_sub512(unsigned char *r, const unsigned char *x, const unsigned char *y)
{
    unsigned char i, c;
    unsigned char temp256[32];

    c = 0;

    bigint_sub256(r, &c, x, y);
    bigint_sub256(r+32, &c, x+32, y+32);

    // can overflow twice, need to carry
    for(i=0; i<=1; i++)
    {
        cmask(temp256, l6, &c);
        c = 0;
        bigint_sub256(r+32, &c, r+32, temp256);
    }
}

void bigint_mul256(unsigned char *r, const unsigned char *x, const unsigned char *y)
{
    unsigned char c, k;
    unsigned char temp256[32];

    bigint_mul(r, x, y);
    bigint_mul(r+32, x+16, y+16);
    bigint_mul(temp256, x, y+16);

    c = 0;
    bigint_add256(r+16, &c, r+16, temp256);

    k = c;
    bigint_mul(temp256, x+16, y);

    c = 0;
    bigint_add256(r+16, &c, r+16, temp256);

    k += c;

    carry(r+48, r+48, &k);
}

void bigint_red256(unsigned char *r, const unsigned char *x)
{
    unsigned char i, c;
    unsigned char tempmul[64], tempadd[64];

    for(i=0; i<=63; i++)
    {
        tempadd[i] = x[i];
    }

    c = 0;
    // do 4 times to reduce to 260 bits
    for(i=0; i<=3; i++)
    {
        bigint_mul256(tempmul, tempadd+32, l6);
        bigint_add256(tempadd, &c, tempadd, tempmul);

        carry(tempadd+32, tempmul+32, &c);
        carry(tempadd+48, tempmul+48, &c);
    }

    // finalize the last bits    
    // use the top part of tempadd, which is now zero
    tempadd[33] = (tempadd[32] & 0x1c) >> 2;
    tempadd[32] = tempadd[32] << 6;
    tempadd[32] |= (tempadd[31] & 0xfc) >> 2;
    tempadd[31] &= 0x03;

    bigint_mul256(tempmul, tempadd+32, l); 
    bigint_add256(tempadd, &c, tempadd, tempmul);

    tempadd[33] = 0;

    // possibly a final reduction
    // set to 0 or 1, depending on bit 250
    // can still be larger than N, but only a little 
    // I.e. bit 250 is zero
    c = (tempadd[31] & 0x04) >> 2;
    tempadd[31] &= 0x03;

    cmask(tempmul, l, &c);
    c = 0;
    bigint_add256(r, &c, tempadd, tempmul);
}
