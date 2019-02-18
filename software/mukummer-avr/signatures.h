#ifndef SIGNATURES_H
#define SIGNATURES_H

static const unsigned char Pdec[64] = { 0x8a, 0x5e, 0xd3, 0x11,
                                        0x62, 0xc7, 0xb8, 0x27,
                                        0xbf, 0x59, 0xe9, 0x07,
                                        0x33, 0x9c, 0x5d, 0x7d, 
                                        0xe8, 0x9c, 0x07, 0x93,
                                        0x2f, 0x30, 0xe8, 0xa7,
                                        0x0c, 0x4e, 0x59, 0x9c,
                                        0x0f, 0x15, 0x03, 0x27,
                                        0x42, 0xc9, 0x88, 0xf2,
                                        0xd8, 0x36, 0x17, 0x72,
                                        0x1c, 0x9c, 0x7a, 0x17,
                                        0xaf, 0x69, 0x45, 0x44,
                                        0x11, 0xfb, 0xae, 0xf8,
                                        0xcf, 0x36, 0x68, 0x31,
                                        0x17, 0x24, 0xf4, 0x25,
                                        0xb2, 0xcf, 0x26, 0x7f };

static const unsigned char xpw[48] = {  0x48, 0x1a, 0x93, 0x4e,
                                        0xa6, 0x51, 0xb3, 0xae,
                                        0xe7, 0xc2, 0x49, 0x20,
                                        0xdc, 0xc3, 0xe0, 0x1b,
                                        0xdf, 0x36, 0x7e, 0xe0,
                                        0x18, 0x98, 0x65, 0x64,
                                        0x30, 0xa6, 0xab, 0x8e,
                                        0xcd, 0x16, 0xb4, 0x23,
                                        0x1e, 0x44, 0x15, 0x72,
                                        0x05, 0x3d, 0xae, 0xc7,
                                        0x4d, 0xa2, 0x47, 0x44,
                                        0x38, 0x5c, 0xb3, 0x5d };

void keygen(unsigned char *Q, unsigned char *dd, const unsigned char *d);

void sign(unsigned char *r, const unsigned char *Q, const unsigned char *m, const unsigned long long inlen, const unsigned char *dd);

unsigned char verify(const unsigned char *Q, const unsigned char *m, const unsigned long long inlen, const unsigned char *sig);

void red(unsigned char *r, const unsigned char *x);

#endif
