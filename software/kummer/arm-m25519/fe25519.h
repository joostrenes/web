#ifndef FE25519_H
#define FE25519_H

#include <inttypes.h>

typedef uint8_t uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;
typedef uint64_t uint64;
typedef int32_t int32;
typedef int64_t int64;

typedef uint32_t crypto_uint32;
typedef uint64_t crypto_uint64;

typedef union
{
    unsigned char b[32];    
    uint32  v[8];
}
fe25519;

void fe25519_freeze(fe25519 *r);
void fe25519_unpack(fe25519 *r, const unsigned char x[32]);
void fe25519_pack(unsigned char r[32], const fe25519 *x);
void fe25519_copy(fe25519 *r, const fe25519 *x);

int fe25519_iszero(const fe25519 *x);
void fe25519_setone(fe25519 *r);
void fe25519_setzero(fe25519 *r);

void fe25519_add(fe25519 *r, const fe25519 *x, const fe25519 *y);
void fe25519_sub(fe25519 *r, const fe25519 *x, const fe25519 *y);
void fe25519_mul(fe25519 *r, const fe25519 *x, const fe25519 *y);
void fe25519_mulconst(fe25519 *r, const fe25519 *x, uint16 y);
void fe25519_square(fe25519 *r, const fe25519 *x);
void fe25519_invert(fe25519 *r, const fe25519 *x);

extern void bigint_mul(uint32 *r, const uint32 *x, const uint32 *y);
extern void bigint_sqr(uint32 *r, const uint32 *x);
extern void bigint_red(fe25519 *r, const uint32 *x);
extern void fe25519_mulconst_asm(fe25519 *r, const fe25519 *x, uint16 y);

#endif
