#ifndef PRINT_H
#define PRINT_H

#include "../fe25519.h"

void write_byte(unsigned char c);

void print(const char *s);

void bigint_print(const unsigned char *x, unsigned char xlen);

void print_stack(const char *primitive, const unsigned int bytes, unsigned int stack);

void print32(const crypto_uint32 *x, unsigned char xlen);

void print8(const unsigned char *x, unsigned char xlen);

#endif
