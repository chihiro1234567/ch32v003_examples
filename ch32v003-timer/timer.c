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

void init_timer2() {
	RCC->APB1PCENR |= RCC_APB1Periph_TIM2;

	// Reset TIM2 to init all regs
	RCC->APB1PRSTR |= RCC_APB1Periph_TIM2;
	RCC->APB1PRSTR &= ~RCC_APB1Periph_TIM2;
  TIM2->CTLR1 |= TIM_ARPE;
	TIM2->CTLR2 = TIM_MMS_1;
	
	// count up per 1sec
	// 48000 * 1000 = 48000000
	TIM2->ATRLR = 48000/3;
	TIM2->PSC = 1000-1;
	TIM2->RPTCR = 0;

	NVIC_EnableIRQ(TIM2_IRQn);

	TIM2->INTFR = ~TIM_FLAG_Update; // 0x0001
	TIM2->SWEVGR = TIM_UG;// 0x0001
	TIM2->DMAINTENR |= TIM_IT_Update; // 0x0001

  // TIM2 Enable
	TIM2->CTLR1 |= TIM_CEN;
}

void TIM2_IRQHandler(void) __attribute__((interrupt));
void TIM2_IRQHandler() {
	if(TIM2->INTFR & TIM_FLAG_Update) {
		TIM2->INTFR = ~TIM_FLAG_Update;
		printf("TIMER2 ISR %lu, %d\n", SysTick->CNT, TIM2->CNT);
	}
}

int main(){

	SystemInit();
	init_timer1();
  init_timer2();
	while(1){
		Delay_Ms(500);
	}
}

