#ifndef KEYEXCHANGE_H
#define KEYEXCHANGE_H

#include "gfe.h"

static const uint32 dh_xbase[12] = { 0x4e931a48, 0xaeb351a6, 0x2049c2e7, 0x1be0c3dc,
                                  0xe07e36df, 0x64659818, 0x8eaba630, 0x23b416cd,
                                  0x7215441e, 0xc7ae3d05, 0x4447a24d, 0x5db35c38};

void dh_exchange(uint32 *r, const uint32 *Q, const uint32 *d);

#endif
