/*
 * Based on crypto_scalarmult/try.c version 20090118 by D. J. Bernstein
 * Public domain.
 */

#include <stdlib.h>
#include "../sign.h"
#include "../dh.h"
#include "print.h"
#include "fail.h"

#include <stdlib.h>
#define loop_min 0
#define loop_len 10
#define mes_len 1

int main(void)
{

    /* Checking signatures */

    unsigned long long i,j;
    unsigned char ch;

    unsigned char sk[64];
    unsigned long long mlen = mes_len;
    unsigned char m[mlen];
    unsigned char pk[32];
    unsigned char sm[64+mlen];
    unsigned long long smlen;

    unsigned long long ctr = 0;
    unsigned char error = 0;
    unsigned char num = loop_len;

    print("\n");
    print("\n");
    print("\n");
    print("\n");
    print("\n");
    print("Checking 0x");
    print8(&num, 1);
    print("signatures........... ");

    for(i=loop_min;i<=loop_min+loop_len-1;i++)
    {
        srand(i);

        for(j=0;j<32;j++) { pk[j] = 0; }
        for(j=0;j<64;j++) { sk[j] = rand() % 256; }
        for(j=0;j<mlen;j++) { m[j] = rand() % 256; }

        keypair(pk, sk);
        sign(sm, &smlen, m, mlen, pk, sk);
        ch = verify(m, mlen, sm, smlen, pk);

        if(!ch) { error = i; i = loop_min+loop_len; ctr += 1; }
    }

    if(!ctr) 
    {
        print("Correct");
    }
    else
    {
        print("\n");
        print("Incorrect!");
        print("\n");
        print8(&error, 1);
        print("error!");
        print("\n");
        print8(sk, 32);
        print("\n");
        print8(sk+32, 32);
        print("\n");
        print8(pk, 32);
        print("\n");
        print8(m, mlen);
        print("\n");
    }

    /* Checking DH */

    ecp r;
    fe25519 c;
    group_scalar n;
    group_scalar ell = {
        .b = { 0x24,0x8F,0x3B,0x2B,0xA5,0xE1,0x4C,0xFA,
               0xD4,0x40,0xA5,0xAA,0xE3,0x83,0x24,0xEB,
               0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
               0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x7F }
    };

    /* Checking 0*base = (a^2 : b^2) */

    print("\nChecking 0*base = (a^2 : b^2)...... ");

    for (i=0; i<32; i++) n.b[i] = 0;

    ladder_base(&r,&n);
    fe25519_mulconst(&r.X, &r.X, 0x0005); // b^2
    fe25519_mulconst(&r.Z, &r.Z, 0x009F); // a^2
    fe25519_sub(&c, &r.X, &r.Z);
    fe25519_freeze(&c);

    if (fe25519_iszero(&c)) print("Correct");

    /* Checking 1*base = base */

    print("\nChecking 1*base = base............. ");

    for (i=0; i<32; i++) n.b[i] = 0;
    n.b[0] = 1;

    ladder_base(&r,&n);
    compress(&c,&r);
    fe25519_freeze(&c);
    c.b[0] = c.b[0] - 7;

    if (fe25519_iszero(&c)) print("Correct");

    /* Checking ell*base = (a^2 : b^2) */

    print("\nChecking ell*base = (a^2 : b^2).... ");

    ladder_base(&r,&ell);
    fe25519_mulconst(&r.X, &r.X, 0x0005); // b^2
    fe25519_mulconst(&r.Z, &r.Z, 0x009F); // a^2
    fe25519_sub(&c, &r.X, &r.Z);
    fe25519_freeze(&c);

    if (fe25519_iszero(&c)) print("Correct");

    /* Checking some key exchanges.. */

    unsigned char dh_sk0[32], dh_sk1[32];
    unsigned char dh_pk0[32], dh_pk1[32];
    unsigned char dh_ss0[32], dh_ss1[32];

    print("\nChecking 0x");
    print8(&num, 1);
    print("DH exchanges......... ");
    for(j=loop_min;j<=loop_min+loop_len-1;j++)
    {
        srand(j);

        for(i=0;i<32;i++) { dh_sk0[i] = rand() % 256; }
        for(i=0;i<32;i++) { dh_sk1[i] = rand() % 256; }

        dh_keygen(dh_pk0, dh_sk0);
        dh_keygen(dh_pk1, dh_sk1);

        dh_exchange(dh_ss0, dh_pk1, dh_sk0);
        dh_exchange(dh_ss1, dh_pk0, dh_sk1);

        for(i=0;i<32;i++) { dh_ss0[i] = dh_ss0[i] - dh_ss1[i]; }
        for(i=0;i<32;i++) { dh_ss0[0] = dh_ss0[0] | dh_ss0[i]; }

        if( dh_ss0[0] ) print("Incorrect!");
    }
    print("Correct\n");

#if 0
    int i;
    fe25519 r;
    fe25519 x = { .b = 
    { 0x97,0x8E,0x8D,0x3D,0x14,0xC,0x7A,0x14,
      0x82,0xF7,0x39,0x69,0xB6,0xAA,0x3A,0xC4,
      0x94,0xFE,0x8A,0x2,0xA,0xCF,0x82,0x1F,
      0xB4,0xA1,0xEA,0xE5,0xCA,0x3F,0x84,0x6C } };
    uint32 y = 0x01FF;

    for(i=0;i<8;i++) { r.v[i] = 0; }

    fe25519_mulconst_(&r, &x, y);

    print("\n");
    print("\n");
    print("\n");
    print32(x.v, 8);
    print("\n");
    print32(r.v, 8);
    print("\n");
#endif

#if 0
    print("\n");
    print("\n");

    fe25519 r;
    fe25519 x = {
        .v = {0,0,0,0,0,0,0,0x80000000}
    };
    fe25519 y = {
        .v = {2,0,0,0,0,0,0,0}
    };

    //fe25519_add(&r, &x, &y);

    print("\n");
    print32(r.v, 8);
    print("\n");

    fe25519_sub(&r, &x, &y);
    fe25519_freeze(&r);

    print("\n");
    print32(r.v, 8);
    print("\n");

    fe25519_mul(&r, &x, &y);
    fe25519_freeze(&r);

    print("\n");
    print32(r.v, 8);
    print("\n");

    int b = 1;
    ecp xp;
    ecp xq;
    fe25519 xd;

    for(i=0;i<8;i++)
    {
        xp.X.v[i] = 0;
        xp.Z.v[i] = 0;
        xq.X.v[i] = 0;
        xq.Z.v[i] = 0;
    }

    xp.X.v[0] = 3;
    xp.Z.v[0] = 4;
    xq.X.v[0] = 1;
    xq.Z.v[0] = 2;

    ecswap(&xp, &xq, b);

    print("\n");
    print32(xp.X.v, 8);
    print("\n");
    print("\n");
    print32(xp.Z.v, 8);
    print("\n");

    print("\n");
    print32(xq.X.v, 8);
    print("\n");
    print("\n");
    print32(xq.Z.v, 8);
    print("\n");

    ecp r;
    group_scalar n;
    fe25519 rx;

    for(i=0;i<8;i++) { n.v[i] = 0; r.X.v[i] = 0; r.Z.v[i] = 0; }
    n.v[0] = 3;

    ladder_base(&r, &n);
    compress(&rx, &r);

    fe25519_freeze(&r.X);
    fe25519_freeze(&r.Z);
    print("\n");
    print32(r.X.v, 8);
    print("\n");
    print("\n");
    print32(r.Z.v, 8);
    print("\n");

    fe25519_freeze(&rx);
    print("\n");
    print32(rx.v, 8);
    print("\n");
#endif

#if 0
    group_scalar r;
    unsigned char sk[64];
    unsigned char pk[32];
    unsigned long long mlen = 1;
    unsigned char m[mlen];
    unsigned char sm[64+mlen];
    unsigned long long smlen;
    int ch;

    for(i=0;i<64;i++) sk[i] = 0;
    sk[0] = 2;

    keypair(pk, sk);

    print("\n");
    print("pk");
    print("\n");
    print8(pk, 32);
    print("\n");

    print("\n");
    print("sk");
    print("\n");
    print8(sk, 64);
    print("\n");

    sm[64] = 0;
    sign(sm, &smlen, sm+64, mlen, pk, sk);

    print("\n");
    print("sm");
    print("\n");
    print8(sm, 64);
    print("\n");

    ch = verify(sm+64, mlen, sm, smlen, pk);

    print("\n");
    if ( ch ) { print("SUCCESS"); }
    print("\n");
#endif

    /*
    gscalar r;
    group_scalar rg;
    unsigned char pk[32];
    unsigned char sk[64];
    for(i=0;i<32;i++) pk[i] = 0;
    for(i=0;i<64;i++) sk[i] = 0;

    pk[0] = 0;
    pk[1] = 1;
    pk[3] = 3;
    pk[4] = 4;

    gscalar_get32(&r, pk);

    print("\n");
    print32(r.v, 8);
    print("\n");

    gscalar_get64(&r, sk);

    print("\n");
    print32(r.v, 8);
    print("\n");

    group_scalar_get64(&rg, sk);

    print("\n");
    print32(rg.v, 32);
    print("\n");
    */

    /*
    uint32 x[9], y[9], r[18], red[9];
    for(i=0;i<9;i++) x[i] = 0xffffffff;
    for(i=0;i<9;i++) y[i] = 0xffffffff;

    bigint_mul9(r, x, y);

    print("\n");
    print32(r, 18);
    print("\n");

    print("\n");
    print32(r, 18);
    print("\n");

    for(i=0;i<18;i++) r[i] = 0;
    r[7] = 0x00000001;
    barrett_reduction(red, r);

    print("\n");
    print32(red, 9);
    print("\n");
    */

    /*
    unsigned char r[32], x[64];
    for(i=0;i<64;i++) x[i] = i;
    x[44] = 0x80;

    barrett_reduction(r, x);
    */

    print("\n");
    print("\n");
    print("\n");
    print("\n");
    print("\n");
    print("\n");

  write_byte(4);

  while(1);
}
