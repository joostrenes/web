#include "cref_print.h"

void fe25519_print(const fe25519 *x)
{
  int i,j;

	for(j=0;j<4;j++)
	{
		printf("{");
		for(i=8*j;i<8*(j+1)-1;i++)
			printf("0x%02X,",x->b[i]);
		printf("0x%02X};\n",x->b[i]);
	}
}
