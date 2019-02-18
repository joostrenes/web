#include <stdlib.h>
#include "../signatures.h"
#include "../keyexchange.h"
#include "../jac.h"
#include "../kummer.h"
#include "../gfe.h"
#include "print.h"
#include "cpucycles.h"
#include "fail.h"
#include "avr.h"

#define NRUNS 6
#define meslen 1

int main(void)
{
  unsigned int i;
  unsigned long long inlen = meslen;
  unsigned long long t[NRUNS];

  int len = 250;
  char bit;
  unsigned char *array;
  unsigned char *bigint;

  bigint = malloc(32);
  if( !bigint ) fail("allocation of bigint failed");
  array = malloc(128);
  if( !array ) fail("allocation of array failed");

  // signatures
  print("\n");
  print("signatures");
  print("\n");
  print("------------------");
  print("\n");

  for(i=0;i<NRUNS;i++)
  {
    t[i] = cpucycles();
    keygen(array, array, array);
  }

  print_speed("keygen",-1,t,NRUNS);

  for(i=0;i<NRUNS;i++)
  {
    t[i] = cpucycles();
    sign(array, array, array, inlen, array);
  }

  print_speed("sign",-1,t,NRUNS);

  for(i=0;i<NRUNS;i++)
  {
    t[i] = cpucycles();
    verify(array, array, inlen, array);
  }

  print_speed("verify",-1,t,NRUNS);

  // key exchange
  print("\n");
  print("key exchange");
  print("\n");
  print("------------------");
  print("\n");

  for(i=0;i<NRUNS;i++)
  {
    t[i] = cpucycles();
    dh_exchange(array, array, array);
  }

  print_speed("dh_exchange",-1,t,NRUNS);

  // jacobian functions
  print("\n");
  print("jacobian");
  print("\n");
  print("------------------");
  print("\n");

  for(i=0;i<NRUNS;i++)
  {
    t[i] = cpucycles();
    jacobian_scalarmult(array, array, array, array);
  }

  print_speed("jacobian_scalarmult",-1,t,NRUNS);

  for(i=0;i<NRUNS;i++)
  {
    t[i] = cpucycles();
    twodim_scalarmult(array, array, array, array, array, array);
  }

  print_speed("twodim_scalarmult",-1,t,NRUNS);

  for(i=0;i<NRUNS;i++)
  {
    t[i] = cpucycles();
    ADD(array, array, array);
  }

  print_speed("ADD",-1,t,NRUNS);

  for(i=0;i<NRUNS;i++)
  {
    t[i] = cpucycles();
    compress(array, array);
  }

  print_speed("compress",-1,t,NRUNS);

  for(i=0;i<NRUNS;i++)
  {
    t[i] = cpucycles();
    decompress(array, array);
  }

  print_speed("decompress",-1,t,NRUNS);

  for(i=0;i<NRUNS;i++)
  {
    t[i] = cpucycles();
    project(array, array);
  }

  print_speed("project",-1,t,NRUNS);

  // kummer functions
  print("\n");
  print("kummer");
  print("\n");
  print("------------------");
  print("\n");

  for(i=0;i<NRUNS;i++)
  {
    t[i] = cpucycles();
    crypto_scalarmult(array, array, array, len);
  }

  print_speed("crypto_scalarmult",-1,t,NRUNS);

  for(i=0;i<NRUNS;i++)
  {
    t[i] = cpucycles();
    xDBLADD(array, array, array, array, array);
  }

  print_speed("xDBLADD",-1,t,NRUNS);

  for(i=0;i<NRUNS;i++)
  {
    t[i] = cpucycles();
    xADD(array, array, array, array);
  }

  print_speed("xADD",-1,t,NRUNS);

  for(i=0;i<NRUNS;i++)
  {
    t[i] = cpucycles();
    xWRAP(array, array);
  }

  print_speed("xWRAP",-1,t,NRUNS);

  for(i=0;i<NRUNS;i++)
  {
    t[i] = cpucycles();
    xUNWRAP(array, array);
  }

  print_speed("xUNWRAP",-1,t,NRUNS);

  for(i=0;i<NRUNS;i++)
  {
    t[i] = cpucycles();
    fast2genPartial(array, array);
  }

  print_speed("fast2genPartial",-1,t,NRUNS);

  for(i=0;i<NRUNS;i++)
  {
    t[i] = cpucycles();
    fast2genFull(array, array);
  }

  print_speed("fast2genFull",-1,t,NRUNS);

  for(i=0;i<NRUNS;i++)
  {
    t[i] = cpucycles();
    recoverFast(array, array, array, array, array);
  }

  print_speed("recoverFast",-1,t,NRUNS);

  for(i=0;i<NRUNS;i++)
  {
    t[i] = cpucycles();
    recoverGeneral(array, array, array, array, array, array);
  }

  print_speed("recoverGeneral",-1,t,NRUNS);

  // bigint & gfe arithmetic
  print("\n");
  print("bigint & gfe arithmetic");
  print("\n");
  print("------------------");
  print("\n");

  for(i=0;i<NRUNS;i++)
  {
    t[i] = cpucycles();
    bigint_mul(array, bigint, bigint);
  }

  print_speed("bigint_mul",-1,t,NRUNS);

  for(i=0;i<NRUNS;i++)
  {
    t[i] = cpucycles();
    bigint_sqr(array, bigint);
  }

  print_speed("bigint_sqr",-1,t,NRUNS);

  for(i=0;i<NRUNS;i++)
  {
    t[i] = cpucycles();
    bigint_red(bigint, array);
  }

  print_speed("bigint_red",-1,t,NRUNS);

  for(i=0;i<NRUNS;i++)
  {
    t[i] = cpucycles();
    gfe_add(bigint, bigint, bigint);
  }

  print_speed("gfe_add",-1,t,NRUNS);

  for(i=0;i<NRUNS;i++)
  {
    t[i] = cpucycles();
    gfe_sub(bigint, bigint, bigint);
  }

  print_speed("gfe_sub",-1,t,NRUNS);

  for(i=0;i<NRUNS;i++)
  {
    t[i] = cpucycles();
    gfe_neg(bigint, bigint);
  }

  print_speed("gfe_neg",-1,t,NRUNS);

  for(i=0;i<NRUNS;i++)
  {
    t[i] = cpucycles();
    gfe_mul(bigint, bigint, bigint);
  }

  print_speed("gfe_mul",-1,t,NRUNS);

  for(i=0;i<NRUNS;i++)
  {
    t[i] = cpucycles();
    gfe_square(bigint, bigint);
  }

  print_speed("gfe_square",-1,t,NRUNS);

  for(i=0;i<NRUNS;i++)
  {
    t[i] = cpucycles();
    gfe_mulconst(bigint, bigint, bigint);
  }

  print_speed("gfe_mulconst",-1,t,NRUNS);

  for(i=0;i<NRUNS;i++)
  {
    t[i] = cpucycles();
    gfe_invert(bigint, bigint);
  }

  print_speed("gfe_invert",-1,t,NRUNS);

  for(i=0;i<NRUNS;i++)
  {
    t[i] = cpucycles();
    gfe_sqrtinv(bigint, bigint, bigint, bigint, bit);
  }

  print_speed("gfe_sqrtinv",-1,t,NRUNS);

  for(i=0;i<NRUNS;i++)
  {
    t[i] = cpucycles();
    gfe_powminhalf(bigint, bigint);
  }

  print_speed("gfe_powminhalf",-1,t,NRUNS);

  for(i=0;i<NRUNS;i++)
  {
    t[i] = cpucycles();
    hadamard(array, array);
  }

  print_speed("hadamard",-1,t,NRUNS);

  free(bigint);

  avr_end();
  return 0;
}
