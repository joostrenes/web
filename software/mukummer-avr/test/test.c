/*
 * Based on crypto_scalarmult/try.c version 20090118 by D. J. Bernstein
 * Public domain.
 */

#include <stdlib.h>
#include "../signatures.h"
#include "../keyexchange.h"
#include "print.h"
#include "avr.h"
#include "fail.h"

/*
static unsigned char *test;
#define testlen 800 
*/

#define keylen 32
#define Qlen 32
#define ddlen 64
#define meslen 1
#define siglen 48
#define checklen 64

static unsigned char privkey[keylen] = 
                    {0x03,0,0,0,0,0,0,0,
                        0,0,0,0,0,0,0,0,
                        0,0,0,0,0,0,0,0,
                        0,0,0,0,0,0,0,0};

// 1
static unsigned char message[meslen] = {0};
// 5
//static const unsigned char message[meslen] = {0,0,0,0,0};
// 32
//static unsigned char message[meslen] = {0,0,0,0,0,0,0,0,
//                                              0,0,0,0,0,0,0,0,
//                                              0,0,0,0,0,0,0,0,
//                                              0,0,0,0,0,0,0,0};

int main(void)
{
    long i;
    long j;

    static unsigned char Q[Qlen];
    static unsigned char dd[ddlen];
    static unsigned char sig[siglen];
    //static unsigned char check[checklen];
    static unsigned char resultbit;

  long start = 0;
  unsigned char num = 5;
  long ctr = 0;
  unsigned char error = 0;
  for(i=start;i<=start+num-1;i++)
  {
    srand(i);

    for(j=0;j<=31;j++)
    {
        privkey[j] = rand() % 256;
    }
    for(j=0;j<=meslen-1;j++)
    {
        message[j] = rand() % 256;
    }

    keygen(Q, dd, privkey);
    sign(sig, Q, message, meslen, dd);
    resultbit = verify(Q, message, meslen, sig);

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
    print_bytes(&error, 1);
    print("error!");
    print("\n");
    print_bytes(privkey, 32);
    print("\n");
    print_bytes(message, 1);
    print("\n");
  }
  else
  {
    print("signatures correct!");
    print("\n");
    print_bytes(&num, 1);
    print("iterations (in hex)");
    print("\n");
  }

  #if 0
  privkey[31] = 0x92;
  privkey[30] = 0x99;
  privkey[29] = 0x07;
  privkey[28] = 0x88;
  privkey[27] = 0xd6;
  privkey[26] = 0x6b;
  privkey[25] = 0xf5;
  privkey[24] = 0x58;
  privkey[23] = 0x05;
  privkey[22] = 0x2d;
  privkey[21] = 0x11;
  privkey[20] = 0x2f;
  privkey[19] = 0x54;
  privkey[18] = 0x98;
  privkey[17] = 0x11;
  privkey[16] = 0x17;
  privkey[15] = 0x47;
  privkey[14] = 0xb3;
  privkey[13] = 0xe2;
  privkey[12] = 0x8c;
  privkey[11] = 0x55;
  privkey[10] = 0x98;
  privkey[9] = 0x4d;
  privkey[8] = 0x43;
  privkey[7] = 0xfe;
  privkey[6] = 0xd8;
  privkey[5] = 0xc8;
  privkey[4] = 0x82;
  privkey[3] = 0x2a;
  privkey[2] = 0xd9;
  privkey[1] = 0xf1;
  privkey[0] = 0xa7;

  /*
  message[63] = 0xae;
  message[62] = 0x20;
  message[61] = 0xe2;
  message[60] = 0x51;
  message[59] = 0xc3;
  message[58] = 0x6c;
  message[57] = 0xfa;
  message[56] = 0x5b;
  message[55] = 0xe4;
  message[54] = 0xd9;
  message[53] = 0xf3;
  message[52] = 0xd5;
  message[51] = 0xa5;
  message[50] = 0xed;
  message[49] = 0xfe;
  message[48] = 0xd3;
  message[47] = 0x05;
  message[46] = 0xa1;
  message[45] = 0xe8;
  message[44] = 0xf7;
  message[43] = 0xf6;
  message[42] = 0x39;
  message[41] = 0x4d;
  message[40] = 0x9b;
  message[39] = 0xe5;
  message[38] = 0x8a;
  message[37] = 0x15;
  message[36] = 0xc4;
  message[35] = 0xb0;
  message[34] = 0xf1;
  message[33] = 0xc5;
  message[32] = 0xe9;
  message[31] = 0x48;
  message[30] = 0xe5;
  message[29] = 0xca;
  message[28] = 0x2a;
  message[27] = 0x67;
  message[26] = 0x5a;
  message[25] = 0xb4;
  message[24] = 0x9c;
  message[23] = 0xa2;
  message[22] = 0x14;
  message[21] = 0xb8;
  message[20] = 0x84;
  message[19] = 0x81;
  message[18] = 0x39;
  message[17] = 0x35;
  message[16] = 0x02;
  message[15] = 0x4b;
  message[14] = 0x0c;
  message[13] = 0x61;
  message[12] = 0xed;
  message[11] = 0xc8;
  message[10] = 0xd1;
  message[9] = 0x30;
  message[8] = 0x5f;
  message[7] = 0xe5;
  message[6] = 0x23;
  message[5] = 0x0d;
  message[4] = 0xf3;
  message[3] = 0x41;
  message[2] = 0x62;
  message[1] = 0x33;
  */
  
  message[0] = 0x48;

    keygen(Q, dd, privkey);

    print("\n");
    print("dd");
    print("\n");
    print_bytes(dd, 16);
    print("\n");
    print_bytes(dd+16, 16);
    print("\n");
    print_bytes(dd+32, 16);
    print("\n");
    print_bytes(dd+48, 16);
    print("\n");

    print("\n");
    print("Q");
    print("\n");
    print_bytes(Q, 16);
    print("\n");
    print_bytes(Q+16, 16);
    print("\n");

    sign(sig, Q, message, meslen, dd);

    print("\n");
    print("sig");
    print("\n");
    print_bytes(sig, 16);
    print("\n");
    print_bytes(sig+16, 16);
    print("\n");
    print_bytes(sig+32, 16);
    print("\n");

    resultbit = verify(check, Q, message, meslen, sig);

    print("\n");
    print("verification");
    print("\n");
    print_bytes(check, 16);
    print("\n");
    print_bytes(check+16, 16);
    print("\n");
    print_bytes(check+32, 16);
    print("\n");
    print_bytes(check+48, 16);
    print("\n");
    print("verification bit:");
    print("\n");
    print_bytes(&resultbit, 1);
    print("\n");

    dd[0] = 0x03;
    for(i=1;i<=31;i++)
    {
        dd[i] = 0;
    }

    dh_exchange(sig, xpw, privkey);

    print("\n");
    print("dh_exchange");
    print("\n");
    print_bytes(sig, 16);
    print("\n");
    print_bytes(sig+16, 16);
    print("\n");
    print_bytes(sig+32, 16);
    print("\n");
    #endif

  avr_end();

  return 0;
}
