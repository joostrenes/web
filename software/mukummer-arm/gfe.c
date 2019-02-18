#include "gfe.h"

void gfe_mul16(gfe r, cgfe a)
{
    unsigned char i;

    for(i=0; i<=6; i++)
    {
        r[7-i] = a[7-i] << 4;
        r[7-i] |= (a[6-i]&0xf0000000)>>28;
    }
    r[0] = a[0] << 4;
}

void gfe_mul(gfe r, cgfe a, cgfe b)
{
    uint32 temp[8];

    bigint_mul(temp, a, b);
    bigint_red(r, temp);
}

void gfe_square(gfe r, cgfe a)
{
    uint32 temp[8];

    bigint_sqr(temp, a);
    bigint_red(r, temp);
}

void gfe_sqrtinv(gfe r, gfe s, cgfe x, cgfe y, const char b)
{
   gfe x5;

   gfe_square(x5, y);
   gfe_mul(x5, x5, x); // xy^2
   gfe_powminhalf(r, x5);
   // r = 1 / [sqrt(x)*y]

   gfe_mul(x5, r, x);
   gfe_mul(s, x5, y); // sqrt(x)
   gfe_pack(s, s);
   gfe_mul(r, r, s); // 1/y

   char t = b ^ s[0]; 
   if( t & 1 )
   {
     gfe_neg(s, s);
   }
}

void gfe_powminhalf(gfe r, cgfe x)
{
   unsigned char i;

   gfe x2, x3, x6;

   gfe_square(x2, x); // 2
   gfe_mul(x3, x2, x); // 3
   gfe_square(x6, x3); // 6
   gfe_square(x6, x6); // 12
   gfe_mul(x3, x6, x3); // 2^4-1
   gfe_square(x6, x3); // 30
   gfe_mul(x6, x6, x); // 2^5-1

   gfe_square(r, x6); // 2^6-2
   for(i=0;i<=3;i++) gfe_square(r, r); // 2^10-2^5
   gfe_mul(x6, r, x6); // 2^10-1

   gfe_square(r, x6); // 2^11-2
   for(i=0;i<=8;i++) gfe_square(r, r); // 2^20-2^10
   gfe_mul(x6, r, x6); // 2^20-1

   gfe_square(r, x6); // 2^21-2
   for(i=0;i<=18;i++) gfe_square(r, r); // 2^40-2^20
   gfe_mul(x6, r, x6); // 2^40-1

   gfe_square(r, x6); // 2^41-2
   for(i=0;i<=38;i++) gfe_square(r, r); // 2^80-2^40
   gfe_mul(r, r, x6); // 2^80-1

   for(i=0;i<=39;i++) gfe_square(r, r); // 2^120-2^40
   gfe_mul(r, r, x6); // 2^120-1

   for(i=0;i<=3;i++) gfe_square(r, r); // 2^124-2^4

   gfe_mul(r, r, x3); // 2^124-1
   gfe_square(r, r); // 2^125-2
   gfe_mul(x6, r, x2); // 2^125
   gfe_square(x6, x6); // 2^126
   gfe_mul(r, r, x6);
}

void gfe_invert(gfe r, cgfe x)
{
  gfe x2,x6,x_10_0;
  int i;

  gfe_square(x2,x);                     //  2 
  gfe_mul(x2,x2,x);                     //  3 mult 
  gfe_square(x6,x2);                    //  6 
  gfe_square(x6,x6);                   //  12 
  gfe_mul(x2,x6,x2);                 //  15 mult 
  gfe_square(x2, x2);                 //  30 
  gfe_mul(x2, x2, x);               //  31 = 2^5-1 mult 

  gfe_square(x6, x2);
  for(i=6;i<10;i++) gfe_square(x6, x6);   //  2^10-2^5 
  gfe_mul(x_10_0,x6,x2);             //  2^10-1 mult 

  gfe_square(x6, x_10_0);
  for(i=11;i<20;i++) gfe_square(x6, x6);  //  2^20-2^10 
  gfe_mul(x_10_0,x6,x_10_0);            //  2^20-1 mult 

  gfe_square(x6, x_10_0);
  for(i=21;i<40;i++) gfe_square(x6, x6);  //  2^40-2^20 
  gfe_mul(x_10_0,x6,x_10_0);            //  2^40-1 mult 

  gfe_square(x6, x_10_0);
  for(i=41;i<80;i++) gfe_square(x6, x6);  //  2^80-2^40 
  gfe_mul(x6,x6,x_10_0);            //  2^80-1 mult 

  for(i=80;i<120;i++) gfe_square(x6, x6); //  2^120-2^40 
  gfe_mul(x6,x6,x_10_0);           //  2^120-1 mult 

  for(i=120;i<125;i++) gfe_square(x6, x6);//  2^125-2^5
  gfe_mul(x2, x6, x2); // 2^125-1

  gfe_square(x6, x2);               // 2^126-2^1 
  gfe_square(x6, x6);                     // 2^127-2^2 
  gfe_mul(r,x6,x);                        // 2^127-3 
}
