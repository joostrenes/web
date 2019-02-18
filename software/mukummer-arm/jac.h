#ifndef JAC_H
#define JAC_H

#include "gfe.h"

// Kummer
static const uint32 M01[4] = {0x366658d2, 0x455ced9f, 0x90aaa71b, 0x1c88abbd};
static const uint32 M02[4] = {0x024cde33, 0x0f20ef8c, 0x1ea559e5, 0x3770e3f5};
static const uint32 M03[4] = {0x627ff562, 0x4e3099a4, 0x1905528d, 0x7a4ac62c};
static const uint32 M10[4] = {0x1c4c847a, 0xbcf1915e, 0xf1f88aba, 0x289a1f56};
static const uint32 M11[4] = {0xf1d9bdc2, 0xa1873750, 0x8703baa2, 0x6bb2f054};
static const uint32 M12[4] = {0x7c2c4cb2, 0x95cf3936, 0x0559ff79, 0x4d66b3d4};
static const uint32 M13[4] = {0x67c33b15, 0x5f75bfae, 0x21e4a757, 0x6a351d94};
static const uint32 M20[4] = {0x510db1d5, 0xa9adb3ab, 0x01c248b0, 0x14c29cc2};
static const uint32 M21[4] = {0x73732c5c, 0xfd2d7d1e, 0x26757678, 0x460e6582};
static const uint32 M22[4] = {0xd3672217, 0x002d6c0e, 0xd01948ff, 0x7a759868};
static const uint32 M23[4] = {0x4a586ecf, 0x2c64fb8c, 0x1bcad76c, 0x1821e675};
static const uint32 M30[4] = {0x9d9f2fa0, 0x313c69da, 0x3324f392, 0x47ca7e7e};
static const uint32 M31[4] = {0xc4c1a0be, 0x9d872c4a, 0x99b618db, 0x706b0303};
static const uint32 M32[4] = {0xd170a2bb, 0xec44f7bc, 0x88f105cf, 0x71886062};
static const uint32 M33[4] = {0xf8a5febf, 0x4040c8cc, 0xdfba4449, 0x2cdfa36d};
static const uint32 f4[4] = {0x6463e172, 0x39ad9e9f, 0xbb9dfe2b, 0x219cc3f8};
static const uint32 f3[4] = {0x188df6e8, 0xc47dc236, 0x48b6069c, 0x4b9e333f};
static const uint32 f2[4] = {0x64c39a35, 0x0c9cd1b1, 0x6d9fcc21, 0x73e799e3};
static const uint32 f1[4] = {0x1e4a8d6e, 0xf537cd79, 0x8e0c2f16, 0x1edd6ee4};

void xUNWRAP(uint32 *r, const uint32 *p);

void xWRAP(uint32 *r, const uint32 *p);

void xADD(uint32 *r, const uint32 *x, const uint32 *y, const uint32 *p);

void fast2genPartial(uint32 *r, const uint32 *p);

void fast2genFull(uint32 *r, const uint32 *p);

void recoverFast(uint32 *r, const uint32 *p, const uint32 *xp, const uint32 *xq, const uint32 *xpq);

void recoverGeneral(uint32 *r, const uint32 *p, const uint32 *xp, const uint32 *xq, const uint32 *xqp, const uint32 *xpq); 

// Jacobian

static const uint32 lambda[4] = {0x55555552, 0x55555555, 0x55555555, 0x15555555};
static const uint32 mu[4] = {0x1919a746, 0x05a505c3, 0xb315130e, 0x73e334fb};
static const uint32 nu[4] = {0x2d2d21f3, 0x6b580648, 0x3bf79971, 0x552ab1b6};
static const uint32 nulambda[4] = {0xc64669d1, 0x81694170, 0xecc544c3, 0x5cf8cd3e};
static const uint32 mulambda[4] = {0xdb2ec39f, 0x43756dc0, 0xf39298fe, 0x3bb082e2};
static const uint32 alphadiv5[4] = {0xadad937d, 0x3ebc52ec, 0x2585ffa2, 0x20528d98};

void copy(uint32 *r, const uint32 *x, unsigned long long xlen);

void compress(uint32 *r, const uint32 *x);

void decompress(uint32 *r, const uint32 *x);

void project(uint32 *r, const uint32 *p);

void jacobian_scalarmult(uint32 *r, const uint32 *n, const uint32 *p, const uint32 *xpw);

void ADD(uint32 *r, const uint32 *x, const uint32 *y);

void twodim_scalarmult(uint32 *r, const uint32 *P, const uint32 *xpw, const uint32 *Q, const uint32 *n, const uint32 *m);

#endif
