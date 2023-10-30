/* Template app on which you can build your own. */

#include "ch32v003fun.h"
#include <stdio.h>

int main()
{
	SystemInit();

	while(1)
	{
		printf( "hello\n" );
		Delay_Ms(250);
	}
}

