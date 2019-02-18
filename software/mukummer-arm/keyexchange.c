#include "keyexchange.h"
#include "jac.h"
#include "kummer.h"
#include "modN.h"

// assumes 256-bit secret keys
// can speed up if assume only 250-bit secret keys
void dh_exchange(uint32 *r, const uint32 *Q, const uint32 *d)
{
    uint32 temp[32];

    red(r, d);
    crypto_scalarmult(temp, r, Q, 250);
    xWRAP(r, temp);
}
