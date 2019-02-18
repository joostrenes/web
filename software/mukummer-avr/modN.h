#ifndef MODN_H
#define MODN_H

void bigint_add256(unsigned char *r, unsigned char *carry, const unsigned char *x, const unsigned char *y);

void gfe_sub512(unsigned char *r, const unsigned char *x, const unsigned char *y);

void group_ops(unsigned char *s, const unsigned char *r, const unsigned char *h, const unsigned char *d);

void bigint_mul256(unsigned char *r, const unsigned char *x, const unsigned char *y);

void bigint_red256(unsigned char *r, const unsigned char *x);

#endif
