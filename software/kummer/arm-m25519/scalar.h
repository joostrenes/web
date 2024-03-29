#ifndef SCALAR_H
#define SCALAR_H

#define GROUP_SCALAR_PACKEDBYTES 32
#include "fe25519.h"

typedef fe25519 group_scalar;

void group_scalar_get32(group_scalar *r, const unsigned char x[32]);
void group_scalar_get64(group_scalar *r, const unsigned char x[64]);
void group_scalar_pack(unsigned char r[32], const group_scalar *x);

void group_scalar_sub(group_scalar *r, const group_scalar *x, const group_scalar *y);
void group_scalar_mul(group_scalar *r, const group_scalar *x, const group_scalar *y);
void group_scalar_set_pos(group_scalar *r);

#endif
