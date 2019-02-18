#ifndef GFE_H
#define GFE_H

#include<inttypes.h>

// make sure it is 32 bit
typedef uint32_t uint32;
typedef uint64_t uint64;
// typedef long uint32;

// 4 x 32-bit
typedef uint32 gfe[4];
typedef const uint32 cgfe[4];

// store in Little Endian format
static const uint32 bbccdd = 0x72; // x_0
static const uint32 aaccdd = 0x39; // y_0
static const uint32 aabbdd = 0x42; // z_0
static const uint32 aabbcc = 0x1A2; // t_0

static const uint32 BBCCDD = 0x341; // x'_0
static const uint32 AACCDD = 0x9C3; // y'_0
static const uint32 AABBDD = 0x651; // z'_0
static const uint32 AABBCC = 0x231; // t'_0

static const uint32 acoeff = 0x0b; // a
static const uint32 bcoeff = 0x16; // b
static const uint32 ccoeff = 0x13; // c
static const uint32 dcoeff = 0x03; // d

/* c */

void gfe_mul16(gfe r, cgfe x);

void gfe_mul(gfe r, cgfe a, cgfe b);

void gfe_square(gfe r, cgfe a);

void gfe_invert(gfe r, cgfe x);

void gfe_sqrtinv(gfe r, gfe s, cgfe x, cgfe y, const char b);

void gfe_powminhalf(gfe r, cgfe x);

/* assembler */

extern void gfe_mulconst(uint32 *r, const uint32 *x, const uint32 b);

extern void gfe_add(uint32 *r, const uint32 *x, const uint32 *y);

extern void gfe_sub(uint32 *r, const uint32 *x, const uint32 *y);

extern void hadamard(uint32 *r, const uint32 *x);

extern void gfe_neg(uint32 *r, const uint32 *x);

extern void gfe_pack(uint32 *r, const uint32 *x);

extern void bigint_sqr(uint32 *r, const uint32 *x);

extern void bigint_mul(uint32 *r, const uint32 *x, const uint32 *y);

extern void bigint_red(uint32 *r, const uint32 *a);

#endif
