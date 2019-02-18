#include "jac.h"
#include "Keccak-avr8.h"
#include "modN.h"
#include "gfe.h"
#include "signatures.h"

void red(unsigned char *r, const unsigned char *x)
{
    unsigned char i;
    unsigned char temp[64];

    copy(temp, x, 32);
    for(i=32; i<=63; i++)
    {
        temp[i] = 0;
    }
    bigint_red256(r, temp);
}

char compare16(const unsigned char *x, const unsigned char *y)
{
    unsigned char i;

    for(i=0; i<=15; i++)
    {
        if( x[i] != y[i] )
        {
            return 0;
        }
    }

    return 1;
}

// Q = public key
// dd = d'||d''
// d = secret key
void keygen(unsigned char *Q, unsigned char *dd, const unsigned char *d)
{
    unsigned char Qdec[64];

    // compute hash and copy d' to s
    crypto_hash(dd, d, 32);

    // compute scalar < 250 bits
    red(Qdec, dd+32);
    gfe_mul16(Qdec, Qdec);
    red(Qdec, Qdec);

    jacobian_scalarmult(Qdec, Qdec, Pdec, xpw);
    compress(Q, Qdec);
}

// res = signature
// Q = public key (compressed)
// m = message
// inlen = message length
// dd = d'||d''
void sign(unsigned char *res, const unsigned char *Q, const unsigned char *m, const unsigned long long inlen, const unsigned char *dd)
{
    int i;

    unsigned char hashinput[64+inlen]; 
    unsigned char r[64];

    copy(hashinput, dd, 32);
    copy(hashinput+32, m, inlen);
    crypto_hash(r, hashinput, inlen+32);
    bigint_red256(res, r); 

    jacobian_scalarmult(hashinput, res, Pdec, xpw);

    compress(res, hashinput);
    copy(hashinput, res, 32);
    copy(hashinput+32, Q, 32);
    copy(hashinput+64, m, inlen);
    crypto_hash(hashinput, hashinput, inlen+64);

    for(i=16;i<=63;i++) { hashinput[i] = 0; }
    group_ops(res, r, hashinput, dd+32);

    //copy(res, s, 32);
    copy(res+32, hashinput, 16);
}

// res = verified yes/no
// Q = public key (compressed)
// m = message
// inlen = input length
// sig = signature
unsigned char verify(const unsigned char *Q, const unsigned char *m, const unsigned long long inlen, const unsigned char *sig)
{
    unsigned char hashinput[64+inlen];
    unsigned char Rcom[64];

    decompress(hashinput, Q);
    twodim_scalarmult(Rcom, Pdec, xpw, hashinput, sig, sig+32);
    compress(hashinput, Rcom);

    copy(hashinput+32, Q, 32);
    copy(hashinput+64, m, inlen);
    crypto_hash(Rcom, hashinput, inlen+64);

    return compare16(Rcom, sig+32);
}
