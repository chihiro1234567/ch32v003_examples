#include "ch32v003fun.h"
#include <stdio.h>

void init_tim1_pwm() {
	// TIM1を使って各チャンネルから波形を出力する
	// ※CH2とCH2Nはどちらか一方のみ。TIM1->CCERで決める。

	// TIM1 Remap=00 (default)
	// TIM1_CH2=PD6(1pin)
	// TIM1_CH4=PC4(7pin)
	// TIM1_CH2N=PA2(3pin) 

	// Enable GPIO, Timer1, AFIO
	RCC->APB2PCENR |= RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOD; //gpio
	RCC->APB2PCENR |= RCC_APB2Periph_AFIO; // remap (今回リマップしないのでOFFでもOK)
	RCC->APB2PCENR |= RCC_APB2Periph_TIM1; // timer1

	// GPIO output 10MHz push-pull setting
	// TIM1_CH2=PD6(1pin)
	uint8_t pin_no = 6;
	GPIOD->CFGLR &= ~(0b1111<<(4*pin_no));
	GPIOD->CFGLR |= (GPIO_Speed_10MHz | GPIO_CNF_OUT_PP_AF)<<(4*pin_no);
	// TIM1_CH4=PC4(7pin)
	pin_no = 4;
	GPIOC->CFGLR &= ~(0b1111<<(4*pin_no));
	GPIOC->CFGLR |= (GPIO_Speed_10MHz | GPIO_CNF_OUT_PP_AF)<<(4*pin_no);
	// TIM1_CH2N=PA2(3pin)
	pin_no = 2;
	GPIOA->CFGLR &= ~(0b1111<<(4*pin_no));
	GPIOA->CFGLR |= (GPIO_Speed_10MHz | GPIO_CNF_OUT_PP_AF)<<(4*pin_no);

	// Reset TIM1 to init all regs
	RCC->APB2PRSTR |= RCC_APB2Periph_TIM1;
	RCC->APB2PRSTR &= ~RCC_APB2Periph_TIM1;

	// count 
	TIM1->PSC = 0; // x1
	TIM1->ATRLR = 48000; // (48000x1)/48000000 = 0.001 sec = 1ms
	TIM1->RPTCR = 0; // repeat count (default)

	//TIM1->CTLR1 |= TIM_ARPE;

	// 000: Reset-TIMx_EGRレジスタのUGビットがトリガ出力(TRGO)として使用される。
	// 010: Update - The update event is selected as a trigger input (TRGO). 
	//TIM1->CTLR2 = TIM_MMS_1;
	
	// generate an update event.
	// 10.4.6 Event Generation Register (TIM1_SWEVGR)
	TIM1->SWEVGR = TIM_UG;// 0x0001
	// Capture/Compare

	// 10.4.9 Compare/Capture Enable Register(TIM1->CCER)
	// チャンネルの有効化
	// TIM1_CH2=PD6(1pin)
	TIM1->CCER |= TIM_CC2E | TIM_CC2P;
	// TIM1_CH2N=PA2(3pin)
	//TIM1->CCER |= TIM_CC2NE | TIM_CC2NP;

	// TIM1_CH4=PC4(7pin)
	TIM1->CCER |= TIM_CC4E | TIM_CC4P;
	

	// 10.4.7 Compare/Capture Control Register 1 (TIM1->CHCTLR1) CH1とCH2
	// 10.4.8 Compare/Capture Control Register 2 (TIM1->CHCTLR2) CH3とCH4

	
	// CH4設定
	// CH4 Mode is output, PWM1 (CHCTLR2 CC4S = 00, OC4M = 110)
	//カウントアップ時の場合、コアカウンタがコンペア値より大きい場合はLOW、そうでない場合はHIGH。
  //カウントダウン時の場合、コアカウンタがコンペア値より大きい場合はHIGH、そうでない場合はLOW。
	TIM1->CHCTLR2 &= ~TIM_CC4S //CC4S[1:0] zero clear // CC4S=00: 出力モード
	TIM1->CHCTLR2 &= ~TIM_OC4M //OC4M[2:0] zero clear 
	TIM1->CHCTLR2 |= TIM_OC4M_2 | TIM_OC4M_1; // OC4M=110: PWMモード１

	// CH2設定
	// CH2 Mode is output, PWM1 (CHCTLR1 CC2S = 00, OC2M = 110)
	TIM1->CHCTLR1 &= ~TIM_CC2S //CC2S[1:0] zero clear // CC2S=00: 出力モード
	TIM1->CHCTLR1 &= ~TIM_OC2M //OC2M[2:0] zero clear 
	TIM1->CHCTLR1 |= TIM_OC2M_2 | TIM_OC2M_1;

	// Set the Capture Compare Register 16bit value
	// 50% duty
	// TIM1->ATRLR = 48000, 48000 x 50% = 24000
	TIM1->CH4CVR = 24000;
	TIM1->CH2CVR = 24000;

	// Enable TIM1 outputs
	TIM1->BDTR |= TIM_MOE;

  // TIM1 Enable
	TIM1->CTLR1 |= TIM_CEN;
}

int main(){

	SystemInit();
	init_tim1_pwm();
	while(1){
		Delay_Ms(500);
	}
}

