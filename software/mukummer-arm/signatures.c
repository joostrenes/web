#include "jac.h"
#include "modN.h"
#include "gfe.h"
#include "signatures.h"
#include "crypto_hash.h"
#include "kummer.h"

void copy_bytes(unsigned char *r, const unsigned char *x, const unsigned long long inlen)
{
    unsigned long long i;
    for(i=0; i<=inlen-1; i++)
    {
        r[i] = x[i];
    }
}

char compare4(const uint32 *x, const uint32 *y)
{
    unsigned char i;

    for(i=0; i<=3; i++)
    {
        if( x[i] != y[i] )
        {
            return 0;
        }
    }

    return 1;
}

// Q = output for public key
// dd = output for d'||d''
// P = input for public generator
// d = input for secret key
void keygen(union data256 *Q, union data512 *dd, const union data256 *d)
{
    uint32 Qdec[16];

    // compute hash and copy d' to s
    crypto_hash0(dd->u8, d->u8, 32);

    // compute scalar < 250 bits
    red(Qdec, dd->u32+8);
    gfe_mul16(Qdec, Qdec);
    red(Qdec, Qdec);

    jacobian_scalarmult(Qdec, Qdec, Pdec, xpw);
    compress(Q->u32, Qdec);
}

// res = signature
// P = intput for public generator (compressed)
// Q = input for public key (compressed)
// m = input for message
// inlen = input for message length
// dd = input for d'||d''
void sign(union data384 *res, const union data256 *Q, const unsigned char *m, const unsigned long long inlen, const union data512 *dd)
{
    int i;
    unsigned char hashinput[64+inlen];
    union data512 r, h;

    copy_bytes(hashinput, dd->u8, 32);
    copy_bytes(hashinput+32, m, inlen);
    crypto_hash0(r.u8, hashinput, inlen+32);
    bigint_red256(res->u32, r.u32); 

    jacobian_scalarmult(h.u32, res->u32, Pdec, xpw);

    compress(res->u32, h.u32);
    copy_bytes(hashinput, res->u8, 32);
    copy_bytes(hashinput+32, Q->u8, 32);
    copy_bytes(hashinput+64, m, inlen);
    crypto_hash0(h.u8, hashinput, inlen+64);

    for(i=4;i<=15;i++)
    {
        h.u32[i] = 0;
    }
    group_ops(res->u32, r.u32, h.u32, dd->u32+8);

    copy(res->u32+8, h.u32, 4);
}

// res = verified yes/no
// P = public generator (compressed)
// Q = public key (compressed)
// m = message
// inlen = input length
// sig = signature
unsigned char verify(const union data256 *Q, const unsigned char *m, const unsigned long long inlen, const union data384 *sig)
{
    unsigned char hashinput[64+inlen];
    union data512 t, Qdec;

    decompress(Qdec.u32, Q->u32);
    twodim_scalarmult(t.u32, Pdec, xpw, Qdec.u32, sig->u32, sig->u32+8);
    compress(Qdec.u32, t.u32);

    copy_bytes(hashinput, Qdec.u8, 32);
    copy_bytes(hashinput+32, Q->u8, 32);
    copy_bytes(hashinput+64, m , inlen);
    crypto_hash0(t.u8, hashinput, inlen+64);

    return compare4(t.u32, sig->u32+8);
}
