#include "keyexchange.h"
#include "jac.h"
#include "kummer.h"
#include "signatures.h"

// assumes 256-bit secret keys
// can speed up if assume only 256-bit keys
void dh_exchange(unsigned char *r, const unsigned char *Q, const unsigned char *d)
{
    unsigned char temp[128];

    red(r, d);
    crypto_scalarmult(temp, r, Q, 250);
    xWRAP(r, temp);
}

/*
void dh_keygen(unsigned char *r, const unsigned char *d)
{
    unsigned char temp[64];

    jacobian_scalarmult(temp, d, dh_base, dh_xbase);
    compress(r, temp);
}

void dh_exchange(unsigned char *r, const unsigned char *Q, const unsigned char *d)
{
    unsigned char Qdec[64], qp[64], qpw[48];

    decompress(Qdec, Q);
    project(qp, Qdec);
    xWRAP(qpw, qp);
    jacobian_scalarmult(qp, d, Qdec, qpw);
    compress(r, qp);
}
*/
