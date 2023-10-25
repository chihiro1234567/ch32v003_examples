#include "ch32v003fun.h"
#include <stdio.h>

void init_tim1_pwm() {
	// TIM1を使って各チャンネルから波形を出力する

	// TIM1 Remap=00 (default)
	// TIM1_CH2=PA1(1pin)
	// TIM1_CH2N=PA2(3pin) 
	// TIM1_CH4=PC4(7pin)
	// ※PA1とPA2はコンプリメンタリ（相互補完）

	//-------------------
	// GPIOA, GPIOC, TIM1有効
	//-------------------
	// Enable GPIO, Timer1, AFIO
	RCC->APB2PCENR |= RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOC; //gpio
	//RCC->APB2PCENR |= RCC_APB2Periph_AFIO; // remap (今回リマップしないのでOFFでもOK)
	RCC->APB2PCENR |= RCC_APB2Periph_TIM1; // timer1

	//-------------------
	// GPIO設定
	//-------------------
	uint8_t pin_no = 0;
	// CH2
	// TIM1_CH2=PA1(1pin) OUTPUT
	pin_no = 1;
	GPIOA->CFGLR &= ~(0b1111<<(4*pin_no));
	GPIOA->CFGLR |= (GPIO_Speed_10MHz | GPIO_CNF_OUT_PP_AF)<<(4*pin_no);
	// CH2N
	// TIM1_CH2N=PA2(3pin) OUTPUT
	pin_no = 2;
	GPIOA->CFGLR &= ~(0b1111<<(4*pin_no));
	GPIOA->CFGLR |= (GPIO_Speed_10MHz | GPIO_CNF_OUT_PP_AF)<<(4*pin_no);
	// CH4
	// TIM1_CH4=PC4(7pin) OUTPUT
	pin_no = 4;
	GPIOC->CFGLR &= ~(0b1111<<(4*pin_no));
	GPIOC->CFGLR |= (GPIO_Speed_10MHz | GPIO_CNF_OUT_PP_AF)<<(4*pin_no);

	// TIM1初期化
	RCC->APB2PRSTR |= RCC_APB2Periph_TIM1;
	RCC->APB2PRSTR &= ~RCC_APB2Periph_TIM1;

	// タイマーカウンタ設定 
	TIM1->PSC = 0; // x1
	TIM1->ATRLR = 48000; // (48000x1)/48000000 = 0.001 sec = 1ms
	TIM1->RPTCR = 0; // repeat count (default)

	// update eventの生成
	// 10.4.6 Event Generation Register (TIM1_SWEVGR)
	TIM1->SWEVGR = TIM_UG;// 0x0001
	
	//-------------------
	// PWMチャンネルの有効化
	//-------------------
	// 10.4.9 Compare/Capture Enable Register(TIM1->CCER)
	// CH2
	// TIM1_CH2=PA1(1pin)
	TIM1->CCER |= TIM_CC2E | TIM_CC2P;
	// CH2N
	// TIM1_CH2N=PA2(3pin) 
	TIM1->CCER |= TIM_CC2NE | TIM_CC2NP; //complementary
	// CH4
	// TIM1_CH4=PC4(7pin)
	TIM1->CCER |= TIM_CC4E | TIM_CC4P;

	//-------------------
	// キャプチャ・コンペア設定
	//-------------------
	// CH4設定
	// CH4 Mode is output, PWM1 (CHCTLR2 CC4S = 00, OC4M = 110)
	TIM1->CHCTLR2 &= ~TIM_CC4S; //CC4S[1:0] zero clear // CC4S=00: 出力モード
	TIM1->CHCTLR2 &= ~TIM_OC4M; //OC4M[2:0] zero clear 
	TIM1->CHCTLR2 |= TIM_OC4M_2 | TIM_OC4M_1; // OC4M=110: PWMモード１
	// CH2設定
	// CH2 Mode is output, PWM1 (CHCTLR1 CC2S = 00, OC2M = 110)
	TIM1->CHCTLR1 &= ~TIM_CC2S; //CC2S[1:0] zero clear // CC2S=00: 出力モード
	TIM1->CHCTLR1 &= ~TIM_OC2M; //OC2M[2:0] zero clear 
	TIM1->CHCTLR1 |= TIM_OC2M_2 | TIM_OC2M_1;

	// Set the Capture Compare Register 16bit value
	// TIM1->ATRLR = 48000, 48000 x 50% duty = 24000
	TIM1->CH4CVR = 24000;
	// TIM1->ATRLR = 48000, 48000 x 25% duty = 12000
	TIM1->CH2CVR = 12000;
	
	// Enable TIM1 outputs
	TIM1->BDTR |= TIM_MOE;
  // TIM1 Enable
	TIM1->CTLR1 |= TIM_CEN;
}

int main(){
	SystemInit();
	init_tim1_pwm();
	while(1){
		Delay_Ms(1000);
	}
}

