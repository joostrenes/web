#ifndef GFE_H
#define GFE_H

typedef unsigned char gfe[16];
typedef const unsigned char cgfe[16];

// store in Little Endian format
static const unsigned char bbccdd[2] = {0x72, 0x00}; // x_0 
static const unsigned char aaccdd[2] = {0x39, 0x00}; // y_0
static const unsigned char aabbdd[2] = {0x42, 0x00}; // z_0
static const unsigned char aabbcc[2] = {0xA2, 0x01}; // t_0

static const unsigned char BBCCDD[2] = {0x41, 0x03}; // x'_0
static const unsigned char AACCDD[2] = {0xC3, 0x09}; // y'_0
static const unsigned char AABBDD[2] = {0x51, 0x06}; // z'_0
static const unsigned char AABBCC[2] = {0x31, 0x02}; // t'_0

static const unsigned char acoeff[2] = {0x0b, 0x00}; // a
static const unsigned char bcoeff[2] = {0x16, 0x00}; // b
static const unsigned char ccoeff[2] = {0x13, 0x00}; // c
static const unsigned char dcoeff[2] = {0x03, 0x00}; // d

/* c */

void gfe_mul16(gfe r, cgfe x);

void gfe_mul(gfe r, cgfe x, cgfe y);

void gfe_square(gfe r, cgfe x);

void gfe_invert(gfe r, cgfe x);

void gfe_sqrtinv(gfe r, gfe s, cgfe x, cgfe y, const char b);

void gfe_powminhalf(gfe r, cgfe x);

/* assembler */

extern void gfe_neg(gfe r, cgfe x);

extern void gfe_add(gfe r, cgfe x, cgfe y);

extern void gfe_sub(gfe r, cgfe x, cgfe y);

extern void hadamard(unsigned char *r, const unsigned char *x);

extern void gfe_pack(unsigned char *r, cgfe x);

extern void gfe_mulconst(gfe r, cgfe a, const unsigned char b[2]);

extern void bigint_mul(unsigned char *r, cgfe a, cgfe b);

extern void bigint_sqr(unsigned char *r, cgfe a);

extern void bigint_red(gfe r, const unsigned char *a);

#endif
