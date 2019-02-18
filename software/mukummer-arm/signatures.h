#ifndef SIGNATURES_H
#define SIGNATURES_H

#include "gfe.h"

static const uint32 xpw[12] = { 0x4e931a48, 0xaeb351a6, 0x2049c2e7, 0x1be0c3dc,
                                  0xe07e36df, 0x64659818, 0x8eaba630, 0x23b416cd,
                                  0x7215441e, 0xc7ae3d05, 0x4447a24d, 0x5db35c38};

static const uint32 Pdec[16] = { 0x11d35e8a, 0x27b8c762, 0x07e959bf, 0x7d5d9c33,
                                   0x93079ce8, 0xa7e8302f, 0x9c594e0c, 0x2703150f,
                                   0xf288c942, 0x721736d8, 0x177a9c1c, 0x444569af,
                                   0xf8aefb11, 0x316836cf, 0x25f42417, 0x7f26cfb2};

union data128 
{
    unsigned char u8[16];
    uint32 u32[4];
};

union data256 
{
    unsigned char u8[32];
    uint32 u32[8];
};

union data384 
{
    unsigned char u8[48];
    uint32 u32[12];
};

union data512 
{
    unsigned char u8[64];
    uint32 u32[16];
};

void keygen(union data256 *Q, union data512 *dd, const union data256 *d);

void sign(union data384 *r, const union data256 *Q, const unsigned char *m, const unsigned long long inlen, const union data512 *dd);

unsigned char verify(const union data256 *Q, const unsigned char *m, const unsigned long long inlen, const union data384 *sig);

#endif
