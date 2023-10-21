#include "ch32v003fun.h"
#include <stdio.h>

int main()
{
	SystemInit();

	// Enable GPIOs GPIOC
	RCC->APB2PCENR |= RCC_APB2Periph_GPIOC;

	// 0000000011110000を反転してクリア
	GPIOC->CFGLR &= ~(0xf<<(4*1));
	// 10MHz, Output Push Pull
	GPIOC->CFGLR |= (GPIO_Speed_10MHz | GPIO_CNF_OUT_PP)<<(4*1);

	while(1)
	{
		// Turn on PC1
		GPIOC->BSHR = (1<<1);
		Delay_Ms( 100 );

		// Turn off PC1
		GPIOC->BSHR = (1<<(16+1));
		Delay_Ms( 100 );
	}
}
