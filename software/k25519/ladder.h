#ifndef LADDER_H
#define LADDER_H

#include "scalar.h"
#include "fe25519.h"

typedef struct 
{
    fe25519 X;
    fe25519 Z;
}
ecp;

void cswap(fe25519 *x, fe25519 *y, int b);
void ecswap(ecp *x, ecp *y, int b);
void xDBLADD(ecp *xp, ecp *xq, const fe25519 *xd);
void ladder(ecp *r, ecp *xp, const fe25519 *xpw, const group_scalar *n);
void ladder_base(ecp *r, const group_scalar *n);
void bValues(fe25519 *bZZ, fe25519 *bXZ, fe25519 *bXX, ecp *xp, ecp *xq);
int check(fe25519 *bZZ, fe25519 *bXZ, fe25519 *bXX, fe25519 *rx, ecp *sP);
void compress(fe25519 *r, const ecp *xp);
void decompress(ecp *xp, const fe25519 *r);

#endif
