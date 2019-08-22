#include "../ladder.h"

#ifdef CORTEX_M0 /* Faster swap.. */
    void cswap(fe25519 *x, fe25519 *y, int b)
    {
        int i;
        uint32 db = -b;
        uint32 t;

        for(i=0;i<=7;i++)
        {
            t = x->v[i] ^ y->v[i];
            t &= db;
            x->v[i] ^= t;
            y->v[i] ^= t;
        }
    }
#else
    void cswap(fe25519 *x, fe25519 *y, int b)
    {
        int i;
        unsigned char db = -b;
        unsigned char t;

        for(i=0;i<=31;i++)
        {
            t = (unsigned char)(x->b[i] ^ y->b[i]);
            t &= db;
            x->b[i] ^= t;
            y->b[i] ^= t;
        }
    }
#endif

void ecswap(ecp *x, ecp *y, int b)
{
    cswap(&x->X,&y->X,b);
    cswap(&x->Z,&y->Z,b);
}

void xDBLADD(ecp *xp, ecp *xq, const fe25519 *xd)
{
    /*
     * Simultaneous xDBL and xADD operation on
     * the Montgomery curve.
     *
     * Input: 
     *      xp: proj. x-coordinate on Montgomery curve
     *      xq: proj. x-coordinate on Montgomery curve
     *      xd: affine x-coordinate of difference xp-xq
     *
     * Output: 
     *      xp: proj. x-coordinate of 2*xp
     *      xq: proj. x-coordinate of xp+xq
     */

    fe25519 b0,b1;

    fe25519_add(&b0, &xp->X, &xp->Z);
    fe25519_sub(&b1, &xp->X, &xp->Z);

    fe25519_add(&xp->X, &xq->X, &xq->Z);
    fe25519_sub(&xp->Z, &xq->X, &xq->Z);
    
    fe25519_square(&xq->X, &b0);
    fe25519_square(&xq->Z, &b1);

    fe25519_mul(&b0, &b0, &xp->X);
    fe25519_mul(&b1, &b1, &xp->Z);

    fe25519_mulconst(&xp->X, &xq->X, 0x004D);
    fe25519_mulconst(&xp->Z, &xq->Z, 0x0052);

    fe25519_mulconst(&b0, &b0, 0x004D);
    fe25519_mulconst(&b1, &b1, 0x0052);

    fe25519_add(&xq->X, &xp->X, &xp->Z);
    fe25519_sub(&xq->Z, &xp->X, &xp->Z);

    fe25519_add(&xp->X, &b0, &b1);
    fe25519_sub(&xp->Z, &b0, &b1);

    fe25519_square(&b0, &xq->X);
    fe25519_square(&b1, &xq->Z);

    fe25519_square(&xq->X, &xp->X);
    fe25519_square(&xq->Z, &xp->Z);

    fe25519_mulconst(&xp->X, &b0, 0x0005);
    fe25519_mulconst(&xp->Z, &b1, 0x009F);

    fe25519_mul(&xq->Z, &xq->Z, xd);
}

void ladder(ecp *r, ecp *xp, const fe25519 *xpw, const group_scalar *n)
{
    /*
     * Montgomery ladder computing n*xp via repeated
     * differential additions and constant-time
     * conditional swaps.
     *
     * Input: 
     *      xp: proj. x-coordinate on Montgomery curve
     *      xpw: affine x-coordinate of xp
     *      n: Scalar (max 255-bit)
     *
     * Output: 
     *      xr: proj. x-coordinate of n*xq
     *      xp: proj. x-coordinate of (n+1)*xp
     */

    int i,swap; int bit = 0; int prevbit = 0;

    fe25519_setone(&r->X);
    fe25519_setone(&r->Z);
    fe25519_mulconst(&r->X, &r->X, 0x009F);
    fe25519_mulconst(&r->Z, &r->Z, 0x0005);

    for(i=254; i>=0; i--)
    {
        bit = ( n->b[i>>3] >> ( i & 7 ) ) & 1;
        swap = bit ^ prevbit;
        prevbit = bit;

        ecswap(r, xp, swap);
        xDBLADD(r, xp, xpw);
    }
    ecswap(r, xp, bit);
}

void ladder_base(ecp *r, const group_scalar *n)
{
    ecp base;
    fe25519 basex;

    fe25519_setzero(&base.X);
    fe25519_setone(&base.Z);
    base.X.b[0] = 7;
    fe25519_copy(&basex, &base.X);

    ladder(r, &base, &basex, n);
}

static void kl_to_mont(ecp *r)
{
    fe25519 b0,b1;
    
    fe25519_mulconst(&b0, &r->X, 0x009F);
    fe25519_mulconst(&b1, &r->X, 0x0005);
    fe25519_mulconst(&r->X, &r->Z, 0x0005);
    fe25519_sub(&r->X, &r->X, &b0);
    fe25519_mulconst(&r->Z, &r->Z, 0x009F);
    fe25519_sub(&r->Z, &r->Z, &b1);
}

void bValues(fe25519 *bZZ, fe25519 *bXZ, fe25519 *bXX, 
        ecp *xp, ecp *xq)
{
    /*
     * Three biquadratic forms B_XX, B_XZ and B_ZZ
     * in the coordinates of xp and xq  
     *
     * Input: 
     *      xp: proj. x-coordinate on Montgomery curve
     *      xq: proj. x-coordinate on Montgomery curve
     *
     * Output: 
     *      bZZ: Element B_ZZ of fe25519
     *      bXZ: Element B_XZ of fe25519
     *      bXX: Element B_XX of fe25519
     */

    fe25519 b0,b1,b2;

    kl_to_mont(xp);
    kl_to_mont(xq);

    fe25519_mul(&b0, &xp->X, &xq->X);
    fe25519_mul(&b1, &xp->Z, &xq->Z);
    fe25519_sub(bZZ, &b0, &b1);
    fe25519_square(bZZ, bZZ);
    fe25519_mulconst(bZZ, bZZ, 0x031B);
    fe25519_add(&b0, &b0, &b1);

    fe25519_mul(&b1, &xp->X, &xq->Z);
    fe25519_mul(&b2, &xq->X, &xp->Z);
    fe25519_sub(bXX, &b1, &b2);
    fe25519_square(bXX, bXX);
    fe25519_mulconst(bXX, bXX, 0x031B);
    fe25519_add(bXZ, &b1, &b2);

    fe25519_mul(bXZ, bXZ, &b0);
    fe25519_mulconst(bXZ, bXZ, 0x031B);
    fe25519_mul(&b0, &b1, &b2);
    fe25519_add(&b0, &b0, &b0);
    fe25519_add(&b0, &b0, &b0);
    fe25519_add(&b1, &b0, &b0);
    fe25519_mulconst(&b1, &b1, 0x1729);
    fe25519_mulconst(&b0, &b0, 0x031B);
    fe25519_add(&b0, &b1, &b0);
    fe25519_sub(bXZ, bXZ, &b0);
    fe25519_add(bXZ, bXZ, bXZ);
}

int check(fe25519 *bZZ, fe25519 *bXZ, fe25519 *bXX,
        fe25519 *rx, ecp *sP)
{
    /*
     * Verify whether B_XXrx^2 - B_XZrx + B_ZZ = 0
     *
     * Input: 
     *      bZZ: Biquadratic form B_ZZ
     *      bXZ: Biquadratic form B_XZ
     *      bXX: Biquadratic form B_XX
     *      rx: affine x-coordinate on Montgomery curve
     *
     * Output: 
     *      1 if B_XXrx^2 - B_XZrx + B_ZZ = 0,
     *      0 otherwise
     */

    fe25519 b0, b1;

    fe25519_copy(&sP->X, rx);
    fe25519_setone(&sP->Z);
    kl_to_mont(sP);

    fe25519_square(&b0, &sP->X);
    fe25519_mul(&b0, &b0, bXX);
    fe25519_mul(&b1, &sP->X, &sP->Z);
    fe25519_mul(&b1, &b1, bXZ);
    fe25519_sub(&b0, &b0, &b1);
    fe25519_square(&b1, &sP->Z);
    fe25519_mul(&b1, &b1, bZZ);
    fe25519_add(&b0, &b0, &b1);

    return fe25519_iszero(&b0);
}

void compress(fe25519 *r, const ecp *xp)
{
    /*
     * Compress from projective representation (X : Z)
     * to affine x = X*Z^{p-2}, where p = 2^255-19
     *
     * Input: 
     *      xp: proj. x-coordinate (X : Z)
     *
     * Output: 
     *      r: affine x-coordinate x = X*Z^{p-2}
     */

    fe25519_invert(r, &xp->Z);  
    fe25519_mul(r, &xp->X, r);
    fe25519_freeze(r);
}

void decompress(ecp *xp, const fe25519 *r)
{
    /*
     * Decompress from affine representation x
     * to projective (x : 1)
     *
     * Input: 
     *      r: affine x-coordinate x
     *
     * Output: 
     *      xp: proj. x-coordinate (x : 1)
     */

    fe25519_copy(&xp->X, r);
    fe25519_setone(&xp->Z);
}
