#include <stdio.h>
#include <stm32f0xx.h>
#include "print.h"
#include "../kummer.h"
#include "../gfe.h"
#include "../modN.h"
#include "../crypto_hash.h"
#include "../signatures.h"
#include "../keyexchange.h"

int main(void)
{
  char out[500];

  /*unsigned char secretKeyAlice[32]; =
                                      { 0x77, 0x07, 0x6d, 0x0a, 0x73, 0x18, 0xa5, 0x7d, 0x3c, 0x16,
                                      0xc1, 0x72, 0x51, 0xb2, 0x66, 0x45, 0xdf, 0x4c, 0x2f,
                                      0x87, 0xeb, 0xc0, 0x99, 0x2a, 0xb1, 0x77, 0xfb, 0xa5,
                                      0x1d, 0xb9, 0x2c, 0x2a };

  unsigned char expectedPublicKeyBob[32]; =
                                            { 0xde, 0x9e, 0xdb, 0x7d, 0x7b, 0x7d, 0xc1, 0xb4, 0xd3, 0x5b,
                                            0x61, 0xc2, 0xec, 0xe4, 0x35, 0x37, 0x3f, 0x83, 0x43,
                                            0xc8, 0x5b, 0x78, 0x67, 0x4d, 0xad, 0xfc, 0x7e, 0x14,
                                            0x6f, 0x88, 0x2b, 0x4f };*/

//  unsigned char sharedSecretCalculatedByAlice[32];
  uint32 q[32], p[12];
  uint32 n[4];
  //unsigned char n[32];

  SysTick_Config(12000000);
  SysTick->VAL = 0;
  int ini, fin;

  int i;
  int num = 4;
  int ini_mul[num], fin_mul[num];

  // signatures
  const union data256 privkey;
  //union data512 res;
  union data256 Q;
  union data512 dd;
  union data384 sig;
  unsigned long long meslen = 128;
  unsigned char message[meslen];

  print("\n");
  print("signatures");
  print("\n");
  print("------------------");
  print("\n");

  // ---------------------
  for(i=0;i<=num-1;i++)
  {
      SysTick->VAL = 0;
      ini_mul[i]= SysTick->VAL;
      keygen(&Q, &dd, &privkey);
      fin_mul[i]= SysTick->VAL;
  
      fin_mul[i] = ini_mul[i]-fin_mul[i];
  }

  sprintf(out, "keygen: %d, %d, %d, %d", fin_mul[0], fin_mul[1], fin_mul[2], fin_mul[3]);
  print(out);
  print("\n");
  // ---------------------

  // ---------------------
  for(i=0;i<=num-1;i++)
  {
      SysTick->VAL = 0;
      ini_mul[i]= SysTick->VAL;
      sign(&sig, &Q, message, meslen, &dd);
      fin_mul[i]= SysTick->VAL;
  
      fin_mul[i] = ini_mul[i]-fin_mul[i];
  }

  sprintf(out, "sign: %d, %d, %d, %d", fin_mul[0], fin_mul[1], fin_mul[2], fin_mul[3]);
  print(out);
  print("\n");
  // ---------------------

  // ---------------------
  for(i=0;i<=num-1;i++)
  {
      SysTick->VAL = 0;
      ini_mul[i]= SysTick->VAL;
      verify(&Q, message, meslen, &sig);
      fin_mul[i]= SysTick->VAL;
  
      fin_mul[i] = ini_mul[i]-fin_mul[i];
  }

  sprintf(out, "verify: %d, %d, %d, %d", fin_mul[0], fin_mul[1], fin_mul[2], fin_mul[3]);
  print(out);
  print("\n");
  // ---------------------

  // Diffie-Hellman 
  uint32 dh_pubkey[12], dh_privkey[8];

  print("\n");
  print("DH");
  print("\n");
  print("------------------");
  print("\n");

/*
  // ---------------------
  for(i=0;i<=num-1;i++)
  {
      SysTick->VAL = 0;
      ini_mul[i]= SysTick->VAL;
      dh_keygen(dh_pubkey, dh_privkey);
      fin_mul[i]= SysTick->VAL;
  
      fin_mul[i] = ini_mul[i]-fin_mul[i];
  }

  sprintf(out, "dh_keygen: %d, %d, %d, %d", fin_mul[0], fin_mul[1], fin_mul[2], fin_mul[3]);
  print(out);
  print("\n");
  // ---------------------
  */

  // ---------------------
  for(i=0;i<=num-1;i++)
  {
      SysTick->VAL = 0;
      ini_mul[i]= SysTick->VAL;
      dh_exchange(dh_pubkey, dh_pubkey, dh_privkey);
      fin_mul[i]= SysTick->VAL;
  
      fin_mul[i] = ini_mul[i]-fin_mul[i];
  }

  sprintf(out, "dh_exchange: %d, %d, %d, %d", fin_mul[0], fin_mul[1], fin_mul[2], fin_mul[3]);
  print(out);
  print("\n");
  // ---------------------

  // shake128
  unsigned char hash_out[64];
  unsigned char hash_in[1];
  unsigned long long inlen = 1;

  print("\n");
  print("shake128");
  print("\n");
  print("------------------");
  print("\n");

  // ---------------------
  SysTick->VAL = 0;
  ini= SysTick->VAL;
  crypto_hash0(hash_out, hash_in, inlen);
  fin= SysTick->VAL;

  fin = ini-fin;

  sprintf(out, "shake128: %d.", fin);
  print(out);
  print("\n");
  // ---------------------

  // jacobian functions
  uint32 j_r[16], j_a[16], j_b[16], j_c[8];
  uint32 j_x[12];
  uint32 j_m[8], j_n[8];

  print("\n");
  print("jacobian functions");
  print("\n");
  print("------------------");
  print("\n");

  // ---------------------
  SysTick->VAL = 0;
  ini= SysTick->VAL;
  jacobian_scalarmult(j_r, j_m, j_a, j_x);
  fin= SysTick->VAL;

  fin = ini-fin;

  sprintf(out, "jacobian_scalarmult: %d.", fin);
  print(out);
  print("\n");
  // ---------------------

/*
  // ---------------------
  SysTick->VAL = 0;
  ini= SysTick->VAL;
  jacobian_twodim_mult(j_r, j_a, j_x, j_b, j_n, j_m);
  fin= SysTick->VAL;

  fin = ini-fin;

  sprintf(out, "jacobian_twodim_mult: %d.", fin);
  print(out);
  print("\n");
  // ---------------------
  */

  // ---------------------
  SysTick->VAL = 0;
  ini= SysTick->VAL;
  twodim_scalarmult(j_r, j_a, j_x, j_b, j_n, j_m);
  fin= SysTick->VAL;

  fin = ini-fin;

  sprintf(out, "twodim_scalarmult: %d.", fin);
  print(out);
  print("\n");
  // ---------------------

  // ---------------------
  SysTick->VAL = 0;
  ini= SysTick->VAL;
  ADD(j_r, j_a, j_b);
  fin= SysTick->VAL;

  fin = ini-fin;

  sprintf(out, "ADD: %d.", fin);
  print(out);
  print("\n");
  // ---------------------

  // ---------------------
  SysTick->VAL = 0;
  ini= SysTick->VAL;
  compress(j_c, j_a);
  fin= SysTick->VAL;

  fin = ini-fin;

  sprintf(out, "compress: %d.", fin);
  print(out);
  print("\n");
  // ---------------------

  // ---------------------
  SysTick->VAL = 0;
  ini= SysTick->VAL;
  decompress(j_r, j_c);
  fin= SysTick->VAL;

  fin = ini-fin;

  sprintf(out, "decompress: %d.", fin);
  print(out);
  print("\n");
  // ---------------------

  // ---------------------
  SysTick->VAL = 0;
  ini= SysTick->VAL;
  project(j_r, j_a);
  fin= SysTick->VAL;

  fin = ini-fin;

  sprintf(out, "project: %d.", fin);
  print(out);
  print("\n");
  // ---------------------

/*
  // ---------------------
  SysTick->VAL = 0;
  ini= SysTick->VAL;
  chainEncoding(j_chain, j_m, j_n);
  fin= SysTick->VAL;

  fin = ini-fin;

  sprintf(out, "chainEncoding: %d.", fin);
  print(out);
  print("\n");
  // ---------------------
  */

  // kummer functions 
  uint32 k_r[16], k_s[16], k_a[16], k_b[16];
  uint32 k_w[12];

  print("\n");
  print("kummer functions");
  print("\n");
  print("------------------");
  print("\n");

  // ---------------------
  SysTick->VAL = 0;
  ini= SysTick->VAL;
  crypto_scalarmult(q, n, p, 0xff);
  fin= SysTick->VAL;

  fin = ini-fin;

  sprintf(out, "crypto_scalarmult: %d.", fin);
  print(out);
  print("\n");
  // ---------------------

  // ---------------------
  SysTick->VAL = 0;
  ini= SysTick->VAL;
  xADD(k_r, k_a, k_b, k_w);
  fin= SysTick->VAL;

  fin = ini-fin;

  sprintf(out, "xADD: %d.", fin);
  print(out);
  print("\n");
  // ---------------------

  // ---------------------
  SysTick->VAL = 0;
  ini= SysTick->VAL;
  xDBLADD(k_r, k_s, k_a, k_b, k_w);
  fin= SysTick->VAL;

  fin = ini-fin;

  sprintf(out, "xDBLADD: %d.", fin);
  print(out);
  print("\n");
  // ---------------------

  // ---------------------
  SysTick->VAL = 0;
  ini= SysTick->VAL;
  xWRAP(k_w, k_a);
  fin= SysTick->VAL;

  fin = ini-fin;

  sprintf(out, "xWRAP: %d.", fin);
  print(out);
  print("\n");
  // ---------------------

  // ---------------------
  SysTick->VAL = 0;
  ini= SysTick->VAL;
  xUNWRAP(k_a, k_w);
  fin= SysTick->VAL;

  fin = ini-fin;

  sprintf(out, "xUNWRAP: %d.", fin);
  print(out);
  print("\n");
  // ---------------------

  // ---------------------
  SysTick->VAL = 0;
  ini= SysTick->VAL;
  fast2genPartial(k_w, k_a);
  fin= SysTick->VAL;

  fin = ini-fin;

  sprintf(out, "fast2genPartial: %d.", fin);
  print(out);
  print("\n");
  // ---------------------

  // ---------------------
  SysTick->VAL = 0;
  ini= SysTick->VAL;
  fast2genFull(k_r, k_a);
  fin= SysTick->VAL;

  fin = ini-fin;

  sprintf(out, "fast2genFull: %d.", fin);
  print(out);
  print("\n");
  // ---------------------

  // ---------------------
  SysTick->VAL = 0;
  ini= SysTick->VAL;
  recoverFast(k_r, k_a, k_w, k_s, k_b);
  fin= SysTick->VAL;

  fin = ini-fin;

  sprintf(out, "recoverFast: %d.", fin);
  print(out);
  print("\n");
  // ---------------------

  // ---------------------
  SysTick->VAL = 0;
  ini= SysTick->VAL;
  recoverGeneral(k_r, k_a, k_w, k_s, k_w, k_w);
  fin= SysTick->VAL;

  fin = ini-fin;

  sprintf(out, "recoverGeneral: %d.", fin);
  print(out);
  print("\n");
  // ---------------------

  // bigint & gfe arithmetic
  uint32 gfe_r[4], gfe_s[4], gfe_a[4], gfe_b[4], gfe_const=0;
  uint32 gfe_bigint[8];
  char bit=0;

  print("\n");
  print("bigint & gfe arithmetic");
  print("\n");
  print("------------------");
  print("\n");

  // ---------------------
  SysTick->VAL = 0;
  ini= SysTick->VAL;
  bigint_mul(gfe_bigint, gfe_a, gfe_b);
  fin= SysTick->VAL;

  fin = ini-fin;

  sprintf(out, "bigint_mul: %d.", fin);
  print(out);
  print("\n");
  // ---------------------

  // ---------------------
  SysTick->VAL = 0;
  ini= SysTick->VAL;
  bigint_sqr(gfe_bigint, gfe_a);
  fin= SysTick->VAL;

  fin = ini-fin;

  sprintf(out, "bigint_sqr: %d.", fin);
  print(out);
  print("\n");
  // ---------------------

  // ---------------------
  SysTick->VAL = 0;
  ini= SysTick->VAL;
  bigint_red(gfe_r, gfe_bigint);
  fin= SysTick->VAL;

  fin = ini-fin;

  sprintf(out, "bigint_red: %d.", fin);
  print(out);
  print("\n");
  // ---------------------

  // ---------------------
  SysTick->VAL = 0;
  ini= SysTick->VAL;
  gfe_mul(gfe_r, gfe_a, gfe_b);
  fin= SysTick->VAL;

  fin = ini-fin;

  sprintf(out, "gfe_mul: %d.", fin);
  print(out);
  print("\n");
  // ---------------------

  // ---------------------
  SysTick->VAL = 0;
  ini= SysTick->VAL;
  gfe_square(gfe_r, gfe_a);
  fin= SysTick->VAL;

  fin = ini-fin;

  sprintf(out, "gfe_square: %d.", fin);
  print(out);
  print("\n");
  // ---------------------

  // ---------------------
  SysTick->VAL = 0;
  ini= SysTick->VAL;
  gfe_mulconst(gfe_r, gfe_a, gfe_const);
  fin= SysTick->VAL;

  fin = ini-fin;

  sprintf(out, "gfe_mulconst: %d.", fin);
  print(out);
  print("\n");
  // ---------------------

  // ---------------------
  SysTick->VAL = 0;
  ini= SysTick->VAL;
  gfe_invert(gfe_r, gfe_a);
  fin= SysTick->VAL;

  fin = ini-fin;

  sprintf(out, "gfe_invert: %d.", fin);
  print(out);
  print("\n");
  // ---------------------

  // ---------------------
  SysTick->VAL = 0;
  ini= SysTick->VAL;
  gfe_sqrtinv(gfe_r, gfe_s, gfe_a, gfe_b, bit);
  fin= SysTick->VAL;

  fin = ini-fin;

  sprintf(out, "gfe_sqrtinv: %d.", fin);
  print(out);
  print("\n");
  // ---------------------

  // ---------------------
  SysTick->VAL = 0;
  ini= SysTick->VAL;
  gfe_powminhalf(gfe_r, gfe_a);
  fin= SysTick->VAL;

  fin = ini-fin;

  sprintf(out, "gfe_powminhalf: %d.", fin);
  print(out);
  print("\n");
  // ---------------------

  // ---------------------
  SysTick->VAL = 0;
  ini= SysTick->VAL;
  gfe_mul16(gfe_r, gfe_a);
  fin= SysTick->VAL;

  fin = ini-fin;

  sprintf(out, "gfe_mul16: %d.", fin);
  print(out);
  print("\n");
  // ---------------------

  // ---------------------
  SysTick->VAL = 0;
  ini= SysTick->VAL;
  gfe_add(gfe_r, gfe_a, gfe_b);
  fin= SysTick->VAL;

  fin = ini-fin;

  sprintf(out, "gfe_add: %d.", fin);
  print(out);
  print("\n");
  // ---------------------

  // ---------------------
  SysTick->VAL = 0;
  ini= SysTick->VAL;
  gfe_sub(gfe_r, gfe_a, gfe_b);
  fin= SysTick->VAL;

  fin = ini-fin;

  sprintf(out, "gfe_sub: %d.", fin);
  print(out);
  print("\n");
  // ---------------------

  // ---------------------
  SysTick->VAL = 0;
  ini= SysTick->VAL;
  hadamard(gfe_r, gfe_a);
  fin= SysTick->VAL;

  fin = ini-fin;

  sprintf(out, "hadamard: %d.", fin);
  print(out);
  print("\n");
  // ---------------------

  // ---------------------
  SysTick->VAL = 0;
  ini= SysTick->VAL;
  gfe_neg(gfe_r, gfe_a);
  fin= SysTick->VAL;

  fin = ini-fin;

  sprintf(out, "gfe_neg: %d.", fin);
  print(out);
  print("\n");
  // ---------------------

  // ---------------------
  SysTick->VAL = 0;
  ini= SysTick->VAL;
  gfe_pack(gfe_r, gfe_a);
  fin= SysTick->VAL;

  fin = ini-fin;

  sprintf(out, "gfe_pack: %d.", fin);
  print(out);
  print("\n");
  // ---------------------

  // group arithmetic
  uint32 g_512[16], g_256[8];
  uint32 g_s[4];

  print("\n");
  print("group arithmetic");
  print("\n");
  print("------------------");
  print("\n");

  // ---------------------
  SysTick->VAL = 0;
  ini= SysTick->VAL;
  group_ops(g_s, g_512, g_512, g_256);
  fin= SysTick->VAL;

  fin = ini-fin;

  sprintf(out, "group_ops: %d.", fin);
  print(out);
  print("\n");
  // ---------------------

  // ---------------------
  SysTick->VAL = 0;
  ini= SysTick->VAL;
  bigint_mul256(g_512, g_256, g_256);
  fin= SysTick->VAL;

  fin = ini-fin;

  sprintf(out, "bigint_mul256: %d.", fin);
  print(out);
  print("\n");
  // ---------------------

  // ---------------------
  SysTick->VAL = 0;
  ini= SysTick->VAL;
  bigint_red256(g_256, g_512);
  fin= SysTick->VAL;

  fin = ini-fin;

  sprintf(out, "bigint_red256: %d.", fin);
  print(out);
  print("\n");
  // ---------------------

  // ---------------------
  SysTick->VAL = 0;
  ini= SysTick->VAL;
  gfe_sub512(g_512, g_512, g_512);
  fin= SysTick->VAL;

  fin = ini-fin;

  sprintf(out, "gfe_sub512: %d.", fin);
  print(out);
  print("\n");
  // ---------------------

  write_byte(4);

  while(1);
}

void SysTick_Handler(void)
{
  ;
}
