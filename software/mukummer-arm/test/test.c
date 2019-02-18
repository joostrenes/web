/*
 * Based on crypto_scalarmult/try.c version 20090118 by D. J. Bernstein
 * Public domain.
 */

#include <stdlib.h>
#include "../signatures.h"
#include "../keyexchange.h"
#include "print.h"
#include "fail.h"
#include "randombytes.h"

int main(void)
{
  uint32 i;
  long long j;

  static const uint32 xpw[12] = { 0x4e931a48, 0xaeb351a6, 0x2049c2e7, 0x1be0c3dc,
                                  0xe07e36df, 0x64659818, 0x8eaba630, 0x23b416cd,
                                  0x7215441e, 0xc7ae3d05, 0x4447a24d, 0x5db35c38 };

  union data256 privkey = {0x03,0,0,0,0,0,0,0,
                                 0,0,0,0,0,0,0,0,
                                 0,0,0,0,0,0,0,0,
                                 0,0,0,0,0,0,0,0};
  //union data512 res;
  union data256 Q;
  union data512 dd;
  union data384 sig;
  unsigned long long meslen = 1;
  unsigned char message[meslen];
  for(i=0;i<=meslen-1;i++) { message[i] = 0; }
  unsigned char resultbit;

  uint32 start = 0;
  uint32 num = 100;
  uint32 ctr = 0;
  uint32 error = 0;
  for(i=start;i<=start+num-1;i++)
  {
    srand(i);

    for(j=0;j<=31;j++)
    {
        privkey.u8[j] = rand() % 256;
    }
    for(j=0;j<=meslen-1;j++)
    {
        message[j] = rand() % 256;
    }

    keygen(&Q, &dd, &privkey);
    sign(&sig, &Q, message, meslen, &dd);
    resultbit = verify(&Q, message, meslen, &sig);

    if(!resultbit)
    {
      error = i;
      i = start+num;
      ctr += 1;
    }
  }

  if(ctr)
  {
    print("incorrect!");
    print("\n");
    print32(&error, 1);
    print("error!");
    print("\n");
    print32(privkey.u32, 8);
    print("\n");
  }
  else
  {
    print("signatures correct!");
    print("\n");
    print32(&num, 1);
    print("iterations (hex)");
    print("\n");
  }
  
  #if 0
  keygen(&Q, &dd, &privkey);

  print("\n");
  print("keygen");
  print("\n");
  print("----------");
  print("\n");
  print("dd");
  print("\n");
  print32(dd.u32, 4);
  print("\n");
  print32(dd.u32+4, 4);
  print("\n");
  print32(dd.u32+8, 4);
  print("\n");
  print32(dd.u32+12, 4);
  print("\n");
  print("\n");
  print("Q");
  print("\n");
  print32(Q.u32, 4);
  print("\n");
  print32(Q.u32+4, 4);
  print("\n");
  print("----------");
  print("\n");

  sign(&sig, &Q, message, meslen, &dd);

  print("\n");
  print("signature");
  print("\n");
  print("----------");
  print("\n");
  print("sig");
  print("\n");
  print32(sig.u32, 4);
  print("\n");
  print32(sig.u32+4, 4);
  print("\n");
  print32(sig.u32+8, 4);
  print("\n");
  print("----------");
  print("\n");

  resultbit = verify(&res, &Q, message, meslen, &sig);

  print("\n");
  print("verification");
  print("\n");
  print("----------");
  print("\n");
  print("result");
  print("\n");
  print32(res.u32, 4);
  print("\n");
  print32(res.u32+4, 4);
  print("\n");
  print32(res.u32+8, 4);
  print("\n");
  print32(res.u32+12, 4);
  print("\n");
  print("result yes/no");
  print("\n");
  bigint_printchars(&resultbit, 1);
  print("\n");
  print("----------");
  print("\n");

  uint32 dh_pubkey[12];
  uint32 dh_privkey[8] = {0x3,0,0,0,0,0,0,0};
  uint32 dh_privkey1[8] = {4,0,0,0,0,0,0,0};

  dh_exchange(dh_pubkey, xpw, dh_privkey); 

  print("\n");
  print("DH exchange");
  print("\n");
  print32(dh_pubkey, 4);
  print("\n");
  print32(dh_pubkey+4, 4);
  print("\n");
  print32(dh_pubkey+8, 4);
  print("\n");
  #endif

  print("end");

  print("\n");

  write_byte(4);

  while(1);
}
