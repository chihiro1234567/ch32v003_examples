#include "ch32v003fun.h"
#include <stdio.h>

#define ADC_NUMCHLS 1
volatile uint16_t adc_buffer[ADC_NUMCHLS]={0};

void init_adc(){
	// ADCクロック設定(48MHz / 2 = 24MHz)
	RCC->CFGR0 &= ~RCC_ADCPRE;
	RCC->CFGR0 |= RCC_ADCPRE_DIV2;
	
	// GPIOD, ADC1有効
	RCC->APB2PCENR |=	RCC_APB2Periph_GPIOD | RCC_APB2Periph_ADC1;

	// GPIO設定
	uint8_t pin_no = 0;
	// PD3/A4
	pin_no = 3;
	GPIOD->CFGLR &= ~(0b111<<(4*pin_no));// analog input
	
	// ADC設定リセット
	RCC->APB2PRSTR |= RCC_APB2Periph_ADC1;
	RCC->APB2PRSTR &= ~RCC_APB2Periph_ADC1;
	
	// ADCシーケンス
	uint8_t adc_num = 1;
	uint8_t adc_channel = 4;
	ADC1->RSQR1 = (adc_num-1) << 20;
	ADC1->RSQR2 = 0;
	ADC1->RSQR3 &= ~ADC_SQ1;
	ADC1->RSQR3 |= (adc_channel<<5*0);

	// ADCサイクル
	// sampling cycle = 241 cycles => 111
	ADC1->SAMPTR2 &= ~(0b111<<(3*adc_channel));
	ADC1->SAMPTR2 |= 0b111<<(3*adc_channel);
	
	// ADC-DMA変換設定
	ADC1->CTLR1 |= ADC_SCAN;	//すべてのチャンネル変換
	ADC1->CTLR2 |= ADC_CONT;	//ONしないと、1回しか変換しない
	ADC1->CTLR2 |= ADC_DMA;		//ONしないと、DMA経由でバッファにコピーされない
	ADC1->CTLR2 |= ADC_EXTSEL;//ONしないと、AD変換しない

	// ADC有効
	ADC1->CTLR2 |= ADC_ADON;
	
	// ADCキャリブレーション
	ADC1->CTLR2 |= ADC_RSTCAL;
	while(ADC1->CTLR2 & ADC_RSTCAL);
	ADC1->CTLR2 |= ADC_CAL;
	while(ADC1->CTLR2 & ADC_CAL);

	// ADC開始
	ADC1->CTLR2 |= ADC_SWSTART;
}

void init_dma(){
	
	// DMA有効
	RCC->AHBPCENR |= RCC_AHBPeriph_DMA1;
	
	// ADC1用DMAチャンネル(DMA1_Channel1)の設定
	// ADCペリフェラルからメモリへコピー
	DMA1_Channel1->PADDR = (uint32_t)&ADC1->RDATAR;
	DMA1_Channel1->MADDR = (uint32_t)adc_buffer;
	// データ転送数（バッファのサイズ）
	DMA1_Channel1->CNTR  = ADC_NUMCHLS;

	// 転送方向を設定ペリフェラル => メモリ
	DMA1_Channel1->CFGR |= DMA_M2M_Disable;
	DMA1_Channel1->CFGR |= DMA_DIR_PeripheralSRC;
	// DMAチャンネルの優先度
	DMA1_Channel1->CFGR |= DMA_Priority_VeryHigh;
	// MSIZE[1:0] Memory address data width setting.
	// 16bit [01]0000000000
	DMA1_Channel1->CFGR |= DMA_MemoryDataSize_HalfWord;
	// PSIZE[1:0] Peripheral address data width setting.
	// 16bit [01]00000000
	DMA1_Channel1->CFGR |= DMA_PeripheralDataSize_HalfWord;

	// MINC 1: Enable incremental memory address increment operation.
	// OFFの場合、バッファを切り替えて交互にコピーせずに1つ目のバッファにだけコピー
	DMA1_Channel1->CFGR |= DMA_MemoryInc_Enable;

	// CIRC DMA channel cyclic mode enable.
	// OFFの場合、最初の1回だけしかDMA転送しない
	DMA1_Channel1->CFGR |= DMA_Mode_Circular;
	
	// DMA割込み
	NVIC_EnableIRQ(DMA1_Channel1_IRQn);
	DMA1_Channel1->CFGR |= DMA_IT_TC;

	// DMA1-Channel1有効
	DMA1_Channel1->CFGR |= DMA_CFGR1_EN;
	
}
void DMA1_Channel1_IRQHandler(void) __attribute__((interrupt));
void DMA1_Channel1_IRQHandler(){
	if(DMA1->INTFR & DMA1_FLAG_TC1) {
		DMA1->INTFCR = DMA_CTCIF1;
		//printf("DMA ISR!\r\n");
	}
}

int main(){
	SystemInit();
	
	init_adc();
	init_dma();

	while(1){
		Delay_Ms(250);
		//printf( "%4d %4d %4d [%4ld]\n\r", adc_buffer[0], adc_buffer[1], adc_buffer[2], ADC1->RDATAR);
		printf( "%4d [%4ld]\n\r", adc_buffer[0], ADC1->RDATAR);
	}
}
