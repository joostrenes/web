#include "../sign.h"
#include "../dh.h"
#include "print.h"

#include <stdlib.h>
#include <time.h>

#define mes_len 1
#define loop_min 0
#define loop_len 10

int main(void)
{
    unsigned long long i;

    /* Checking signatures */

    int j;
    unsigned char sk[64]; 
    unsigned long long mlen = mes_len;
    unsigned char m[mlen]; 
    unsigned char pk[32];
    unsigned char sm[64+mlen];
    unsigned long long smlen;
    int ch;

    srand( time(NULL) );
    printf("\nChecking %d signatures............. ", loop_len);
    for(j=loop_min;j<=loop_min+loop_len-1;j++)
    {
        srand(j);

        for(i=0;i<64;i++) { sk[i] = rand() % 256; }
        for(i=0;i<mlen;i++) { m[i] = rand() % 256; }

        keypair(pk, sk);
        sign(sm, &smlen, m, mlen, pk, sk);
        ch = verify(m, mlen, sm, smlen, pk);

        if ( ch != 1 ) 
        {
            printf("\n%lld\n", mlen);
            for(i=0;i<32;i++) { printf("%02X,", sk[i]); }
            printf("\n");
            for(i=0;i<mlen;i++) { printf("%02X,", m[i]); }
        }
    }
    printf("Correct");

    /* End checking signatures */

    /* Checking DH */

    ecp r;
    fe25519 c;
    group_scalar n;
    group_scalar ell = {
        .v = { 0x24,0x8F,0x3B,0x2B,0xA5,0xE1,0x4C,0xFA,
               0xD4,0x40,0xA5,0xAA,0xE3,0x83,0x24,0xEB,
               0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
               0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x7F }
    };

    /* Checking 0*base = (a^2 : b^2) */

    printf("\nChecking 0*base = (a^2 : b^2)...... ");

    for (i=0; i<32; i++) n.v[i] = 0;

    ladder_base(&r,&n);
    fe25519_mulconst(&r.X, &r.X, 0x05, 0x00); // b^2
    fe25519_mulconst(&r.Z, &r.Z, 0x9F, 0x00); // a^2
    fe25519_sub(&c, &r.X, &r.Z);
    fe25519_freeze(&c);

    if (fe25519_iszero(&c)) printf("Correct");

    /* Checking 1*base = base */

    printf("\nChecking 1*base = base............. ");

    for (i=0; i<32; i++) n.v[i] = 0;
    n.v[0] = 1;

    ladder_base(&r,&n);
    compress(&c,&r);
    fe25519_freeze(&c);
    c.v[0] = c.v[0] - 7;

    if (fe25519_iszero(&c)) printf("Correct");

    /* Checking ell*base = (a^2 : b^2) */

    printf("\nChecking ell*base = (a^2 : b^2).... ");

    ladder_base(&r,&ell);
    fe25519_mulconst(&r.X, &r.X, 0x05, 0x00); // b^2
    fe25519_mulconst(&r.Z, &r.Z, 0x9F, 0x00); // a^2
    fe25519_sub(&c, &r.X, &r.Z);
    fe25519_freeze(&c);

    if (fe25519_iszero(&c)) printf("Correct");

    /* Checking some key exchanges.. */

    unsigned char dh_sk0[32], dh_sk1[32];
    unsigned char dh_pk0[32], dh_pk1[32];
    unsigned char dh_ss0[32], dh_ss1[32];

    printf("\nChecking %d DH exchanges........... ", loop_len);
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

        if( dh_ss0[0] ) printf("Incorrect!");
    }
    printf("Correct\n");

    /* End checking DH */

    /* DH */

#if 0
    unsigned char dh_sk[32];
    unsigned char dh_pk[32];
    unsigned char dh_ss[32];
    for(i=0;i<32;i++)
    {
        dh_sk[i] = 0;
        dh_pk[i] = 0;
    }
    dh_sk[31] = 0x40;

    dh_keygen(dh_pk, dh_sk);

    printf("\n");
    for(i=0;i<32;i++) { printf("%02X ", dh_pk[i]); }
    printf("\n");

    for(i=0;i<32;i++) { dh_sk[i] = 0; }
    dh_sk[0] = 2;
    dh_exchange(dh_ss, dh_pk, dh_sk);

    printf("\n");
    for(i=0;i<32;i++) { printf("%02X ", dh_ss[i]); }
    printf("\n");
#endif

    /* end DH */

    /* bValues */

#if 0
    ecp xq, xr, xt;
    fe25519 xqX = {
        .v = 
        {0xF6,0x7,0x48,0xC,0xD7,0xA0,0x1,0x18,
         0x5F,0x41,0x1A,0x2A,0x50,0x48,0xA6,0x10,
         0x4C,0xB5,0x75,0xD9,0xE3,0x1C,0xBE,0x7C,
         0x7F,0xFB,0xE7,0xF4,0x9,0xC9,0x93,0x62 }
    };
    fe25519 xqZ = {
        .v =
        {0x80,0xA7,0x52,0xFD,0xF9,0x46,0x19,0xE,
         0x53,0x15,0x8C,0x8A,0xB9,0x17,0x8F,0xF4,
         0x8D,0x4E,0x31,0xD7,0xF0,0xA4,0xCC,0x9C,
         0xD,0x79,0xB7,0xDE,0x7D,0xE5,0x5F,0x64 }
    };
    fe25519 xrX = {
        .v = 
        {0xE6,0x3D,0xDE,0xC0,0xCA,0xF9,0x31,0xCB,
        0xC0,0x42,0xCC,0x80,0xF5,0x9C,0x45,0xA0,
        0x40,0x72,0xA5,0x27,0xDA,0x75,0x7B,0x4E,
        0xB7,0xF5,0x89,0xF3,0xE6,0xAC,0xAF,0x60 }
    };
    fe25519 xrZ = {
        .v = 
        {0x29,0x5B,0x0,0xFC,0xEB,0xD,0xF1,0x7E,
        0x4C,0x3A,0xA5,0x7F,0x49,0x38,0xD6,0xB9,
        0x7F,0xB5,0x11,0x86,0xE,0x98,0x52,0x79,
        0x34,0xA,0x84,0xE8,0x58,0xE6,0x81,0x57 }
    };
    fe25519 xtX = {
        .v =
        {0x13,0x7D,0xA9,0xD3,0x13,0x5,0xE1,0xA9,
        0x2B,0xB6,0x48,0x2F,0xC8,0x3F,0x1,0x2E,
        0xF4,0x1B,0x7,0x6B,0x6D,0xF9,0x14,0xBD,
        0xE7,0xCB,0x65,0x7C,0xC3,0x9B,0x28,0x66 }
    };
    fe25519 xtZ = {
        .v = 
        {0x89,0xED,0xD6,0xBB,0x27,0x96,0xF0,0x3A,
        0x31,0x54,0x90,0x2E,0xAB,0x9,0x42,0x36,
        0x1E,0xCD,0xE0,0x4F,0xD1,0xA8,0x41,0xE,
        0x3E,0x6A,0x31,0xFE,0x18,0xA3,0x14,0x40 }
    };
    fe25519 bXX,bXZ,bZZ;
    int cha;

    fe25519_copy(&xq.X, &xqX);
    fe25519_copy(&xq.Z, &xqZ);
    fe25519_copy(&xr.X, &xrX);
    fe25519_copy(&xr.Z, &xrZ);
    fe25519_copy(&xt.X, &xtX);
    fe25519_copy(&xt.Z, &xtZ);

    kl_to_mont(&xq);
    kl_to_mont(&xr);
    kl_to_mont(&xt);

    printf("\n");
    for(i=0;i<32;i++) { printf("%02X ", xq.X.v[i]); }
    printf("\n");
    for(i=0;i<32;i++) { printf("%02X ", xq.Z.v[i]); }
    printf("\n");
    printf("\n");
    for(i=0;i<32;i++) { printf("%02X ", xr.X.v[i]); }
    printf("\n");
    for(i=0;i<32;i++) { printf("%02X ", xr.Z.v[i]); }
    printf("\n");
    printf("\n");
    for(i=0;i<32;i++) { printf("%02X ", xt.X.v[i]); }
    printf("\n");
    for(i=0;i<32;i++) { printf("%02X ", xt.Z.v[i]); }
    printf("\n");

    bValues(&bZZ, &bXZ, &bXX, &xq, &xr);
    cha = check(&bZZ, &bXZ, &bXX, &xt);

    printf("%02X ", cha);
    printf("\n");
#endif

    /* end bValues */

    /* scalarmult */

#if 0
    unsigned char scal0[32],scal1[32];
    group_scalar sc0,sc1,sc2;
    for(i=0;i<32;i++) {
        scal0[i] = 0xFF;
        scal1[i] = 0xFF;
    }
    scal0[31] = 0xFF;
    scal1[31] = 0xFF;

    group_scalar_get32(&sc0,scal0);
    group_scalar_get32(&sc1,scal1);
    group_scalar_mul(&sc2,&sc1,&sc0);

    printf("\n");
    for(i=0;i<32;i++) { printf("%02X ", sc0.v[i]); }
    printf("\n");
    for(i=0;i<32;i++) { printf("%02X ", sc1.v[i]); }
    printf("\n");
    for(i=0;i<32;i++) { printf("%02X ", sc2.v[i]); }
    printf("\n");
#endif

    /* end scalarmult */

    return 0;
}
