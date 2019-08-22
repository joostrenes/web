/*
 * Based on crypto_scalarmult/try.c version 20090118 by D. J. Bernstein
 * Public domain.
 */

#include <stdlib.h>
#include "../../dh.h"
#include "../../sign.h"
#include "print.h"
#include "fail.h"

#define loop_min 0
#define loop_len 10
#define mes_len 1

int main(void)
{
    unsigned long long i;

    /* DH */

#if 0
    unsigned char dh_sk[32];
    unsigned char dh_pk[32];
    unsigned char dh_ss[32];
    for(i=0; i<32; i++) {
        dh_sk[i] = 0;
        dh_pk[i] = 0;
        dh_ss[i] = 0;
    }
    dh_sk[31] = 0x50;

    dh_keygen(dh_pk, dh_sk);

    print("\n");
    print("Public key:");
    print("\n");
    print8(dh_pk, 32);
    print("\n");

    for(i=0; i<32; i++) { dh_sk[i] = 0; }
    dh_sk[0] = 2;
    dh_exchange(dh_ss, dh_pk, dh_sk);

    print("\n");
    print("Shared secret:");
    print("\n");
    print8(dh_ss, 32);
    print("\n");
#endif

    /* end DH */

    /* Signatures API */

    unsigned long long j;
    unsigned char ch;

    unsigned char sk[64];
    unsigned long long mlen = mes_len;
    unsigned char m[mlen];
    unsigned char pk[32];
    unsigned char sm[64+mlen];
    unsigned long long smlen;

    unsigned long long ctr = 0;
    unsigned char error = 0;

    for(i=loop_min;i<=loop_min+loop_len-1;i++)
    {
        srand(i);

        for(j=0;j<32;j++) { pk[j] = 0; }
        for(j=0;j<64;j++) { sk[j] = rand() % 256; }
        for(j=0;j<mlen;j++) { m[j] = rand() % 256; }

        /*
        for(j=0;j<64;j++) { sk[j] = 0; }
        for(j=0;j<mlen;j++) { m[j] = 0; }
        sk[0] = i;
        */

        keypair(pk, sk);

        /*
        print("\n");
        print("Public key:");
        print("\n");
        print8(pk, 32);
        print("\n");
        */

        sign(sm, &smlen, m, mlen, pk, sk);

        /*
        print("Signature:");
        print("\n");
        print8(sm, 32);
        print("\n");
        print8(sm+32, 32);
        print("\n");
        */

        ch = verify(m, mlen, sm, smlen, pk);

        if(!ch) { error = i; i = loop_min+loop_len; ctr += 1; }
    }

    if(!ctr) 
    {
        print("\n");
        print("Signatures correct!");
        print("\n");
        unsigned char num = loop_len;
        print8(&num, 1);
        print("iterations (in hex)");
        print("\n");
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

    print("\n");
    print("\n");
    print("\n");
    print("\n");
    print("\n");
    print("\n");

  write_byte(4);

  while(1);
}
