#ifndef MODN_H
#define MODN_H

static const uint32 l6[8] = {0x03016f40, 0xdcc2d2e1, 0x68553fd1, 0xb09ff27e, 0x31d4a534, 0xcd35a608, 0x00000000, 0x00000000};
static const uint32 l[8] = {0x840c05bd, 0x47730b4b, 0xf9a154ff, 0xd2c27fc9, 0x20c75294, 0x0334d698, 0x00000000, 0x00000000};

void bigint_add256(uint32 *r, uint32 *carry, const uint32 *x, const uint32 *y);

void gfe_sub512(uint32 *r, const uint32 *x, const uint32 *y);

void bigint_mul256(uint32 *r, const uint32 *x, const uint32 *y);

// reduces integer with < 512 bits
void bigint_red256(uint32 *r, const uint32 *x);

void group_ops(uint32 *s, const uint32 *r, const uint32 *h, const uint32 *d);

// reduces integer with < 256 bits
void red(uint32 *r, const uint32 *x);

#endif
