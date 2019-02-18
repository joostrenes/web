#ifndef FIPS202_H
#define FIPS202_H

#include "fe25519.h"

union state { unsigned char u8[200]; uint64_t u64[25]; };

extern void KeccakF1600_StateInitialize(void *state);
extern void KeccakF1600_StateXORBytes(void *state, const unsigned char *data, unsigned int offset, unsigned int length);
extern void KeccakF1600_StatePermute( void *state );
extern void KeccakF1600_StateExtractBytes(void *state, const unsigned char *data, unsigned int offset, unsigned int length);

void hash(unsigned char *output, const unsigned char *input, unsigned int inputByteLen);

#endif
