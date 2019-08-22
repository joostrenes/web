#include "../../sign.h"
#include "../../dh.h"
#include "cref_print.h"

#include <stdlib.h>
#include <time.h>

#define mes_len 1
#define loop_min 0
#define loop_len 10

int main(void)
{
    unsigned long long i;

    /* Signatures API */

    int j;
    unsigned char sk[64]; 
    unsigned long long mlen = mes_len;
    unsigned char m[mlen]; 
    unsigned char pk[32];
    unsigned char sm[64+mlen];
    unsigned long long smlen;
    int ch;

    srand( time(NULL) );
    printf("Checking %d signatures...\n", loop_len);
    for(j=loop_min;j<=loop_min+loop_len-1;j++)
    {
        srand(j);

        for(i=0;i<64;i++) { sk[i] = rand() % 256; }
        for(i=0;i<mlen;i++) { m[i] = rand() % 256; }

        //
        for(i=0;i<64;i++) { sk[i] = 0; }
        for(i=0;i<mlen;i++) { m[i] = 0; }
        //

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
    printf("Finished");

    /* end signatures API */

    /* DH */

#if 0
    unsigned char dh_sk[32];
    unsigned char dh_pk[32];
    unsigned char dh_ss[32];
    for(i=0;i<32;i++)
    {
        dh_sk[i] = 0;
        dh_pk[i] = 0;
        dh_ss[i] = 0;
    }
    dh_sk[31] = 0x50;

    dh_keygen(dh_pk, dh_sk);

    printf("\n");
    printf("Public key");
    printf("\n");
    for(i=0;i<32;i++) { printf("%02X ", dh_pk[i]); }
    printf("\n");

    for(i=0;i<32;i++) { dh_sk[i] = 0; }
    dh_sk[0] = 2;
    dh_exchange(dh_ss, dh_pk, dh_sk);

    printf("\n");
    printf("Shared secret");
    printf("\n");
    for(i=0;i<32;i++) { printf("%02X ", dh_ss[i]); }
    printf("\n");
#endif

    /* end DH */

    /* scalarmult */

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
    for(i=0;i<32;i++) { printf("%02X ", sc0.b[i]); }
    printf("\n");
    for(i=0;i<32;i++) { printf("%02X ", sc1.b[i]); }
    printf("\n");
    for(i=0;i<32;i++) { printf("%02X ", sc2.b[i]); }
    printf("\n");

    /* end scalarmult */

    return 0;
}
