#include "ch32v003fun.h"
#include <stdio.h>

void init_timer1() {
	RCC->APB2PCENR |= RCC_APB2Periph_TIM1;
	TIM1->CTLR1 |= TIM_ARPE;
	TIM1->CTLR2 = TIM_MMS_1;
	
	// count up per 1sec
	// 48000 * 1000 = 48000000
	TIM1->ATRLR = 48000;
	TIM1->PSC = 1000-1;
	TIM1->RPTCR = 0;

	NVIC_EnableIRQ(TIM1_UP_IRQn);

	TIM1->INTFR = ~TIM_FLAG_Update; // 0x0001 // 10.4.5 Interrupt Status Register (TIM1_INTFR)
	TIM1->SWEVGR = TIM_UG;// 0x0001 // 10.4.6 Event Generation Register (TIM1_SWEVGR)
	TIM1->DMAINTENR |= TIM_IT_Update; // 0x0001 // 10.4.4 DMA/Interrupt Enable Register (TIM1_DMAINTENR)

  // TIM1 Enable
	TIM1->CTLR1 |= TIM_CEN;
}

void TIM1_UP_IRQHandler(void) __attribute__((interrupt));
void TIM1_UP_IRQHandler() {
	if(TIM1->INTFR & TIM_FLAG_Update) {
		TIM1->INTFR = ~TIM_FLAG_Update;
		printf("TIMER1 ISR %lu, %d\n", SysTick->CNT, TIM1->CNT);
	}
}

int main(){

	SystemInit();
	init_timer1();
	while(1){
		Delay_Ms(500);
	}
}

