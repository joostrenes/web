#ifndef KUMMER_H
#define KUMMER_H

#include "gfe.h"
#include "jac.h"

void crypto_scalarmult(uint32 *r, const uint32 *n, const uint32 *p, const int l);

void xDBLADD(uint32 *x, uint32 *y, const uint32 *z, const uint32 *t, const uint32 *p);

#endif
