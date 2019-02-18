#ifndef KUMMER_H
#define KUMMER_H

#include "gfe.h"

void crypto_scalarmult(unsigned char *r, const unsigned char *n, const unsigned char *p, const int l);

void xDBLADD(unsigned char *x, unsigned char *y, const unsigned char *z, const unsigned char *t, const unsigned char *p);

#endif
