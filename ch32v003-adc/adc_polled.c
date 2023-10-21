/*
 * Example for using ADC with polling
 * 03-27-2023 E. Brombaugh
 */

#include "ch32v003fun.h"
#include <stdio.h>

//---------------------------------------
// ADCの設定
// PA2/A0に対してAD入力を行う
//---------------------------------------
void adc_init( void ){
	// SystemInit()コールすることで
	// 内部オシレータで48MHzで動作する
	
	// ADC clock source prescaler control
	// RCC_ADCPRE:ADCPRE[1:0] bits (ADC prescaler) = 0x0000C000=0b1100000000000000
	RCC->CFGR0 &= ~RCC_ADCPRE;  // Clear ADCPRE[1:0]
	// RCC_ADCPRE_DIV2: PCLK2 divided by 2.
	// ADC Clockには、48MHz / 2 = 24MHzが設定される
	RCC->CFGR0 |= RCC_ADCPRE_DIV2;

	//---------------------------------------
	// GPIOAとADCを有効にする
	//---------------------------------------
	RCC->APB2PCENR |= RCC_APB2Periph_GPIOA | RCC_APB2Periph_ADC1;
	
	//---------------------------------------
	// PA2/A0のアナログ入力設定
	//---------------------------------------
	// 7.3.1.1 Port Configuration Register Low (GPIOx_CFGLR) (x=A/C/D)
	// 各チャンネル(x)毎にCNF(2bit)とMODE(2bit)の合計4bitで構成されている
	// 利用するチャンネルに該当する箇所に設定を行う
	// [CNFx][MODEx]...[CNF0][MODE0]
	// [00][00]...[00][00]
	// x = pin no. (0 - 7)

	// ex.) PD4/A7 => D4, Analog channel7
	// 0b1111<<(4*4) = 1111 0000 0000 0000 0000
	// 4    3    2    1    1
	// 1111 0000 0000 0000 0000
	// 
	// D4をゼロで初期化 => CNF = 00: Analog, MODE = 00: Input
	// uint8_t pin_no = 4;
	// GPIOA->CFGLR &= ~(0b1111<<(4*pin_no));

	// PA2/A0
	uint8_t pin_no = 2;
	GPIOA->CFGLR &= ~(0b1111<<(4*pin_no));
	
	// Reset the ADC to init all regs
	RCC->APB2PRSTR |= RCC_APB2Periph_ADC1;
	RCC->APB2PRSTR &= ~RCC_APB2Periph_ADC1;
	
	//---------------------------------------
	// ADCのシーケンス設定
	//---------------------------------------
	// 9.3.11 ADC Regular Sequence Register 3(ADC_RSQR3)

	// RSQR1～RSQR3の3つのレジスタに渡って、SQ1～SQ16の最大16個のAD変換する順番が定義できる
	// SQ1～SQ16は各4bitのレジスタで0(0000)～9(1001)の値が設定できる
	// この値はチャンネル数に相当しており、PD4/A7であれば7、PA2/A0であれば0を指定する。
	// 例えば3つのアナログデバイスが接続されており、それぞれPA2/A0、PD4/A7, PC4/A2だとする。
	// これをA0、A7、A2の順番に（以降その繰り返し）で変換したいとする。（CH32V003は1個のADCモジュールしかない）
	// その場合、SQ1=0, SQ2=7, SQ3=2に値を設定する。6個まではRSQR3で定義できる、
	// ADC1->RSQR3 $= ~ADC_SQ1
	// ADC1->RSQR3 |= (0<<5*0) // 0を0bitシフトした場所に代入
	// ADC1->RSQR3 $= ~ADC_SQ2
	// ADC1->RSQR3 |= (7<<5*1) // 7を5bitシフトした場所に代入
	// ADC1->RSQR3 $= ~ADC_SQ3
	// ADC1->RSQR3 |= (2<<5*2) // 2を5*2bitシフトした場所に代入
	//
	// RSQR1に個数-1を設定する。(1個の場合は0)
	// uint8_t adc_num = 3; 
	// ADC1->RSQR1 = (adc_num-1) << 20;
	// ADC1->RSQR2 = 0;

	uint8_t adc_num = 1;
	uint8_t adc_channel = 0;
	ADC1->RSQR1 = (adc_num-1) << 20;
	ADC1->RSQR2 = 0;
	ADC1->RSQR3 &= ~ADC_SQ1;
	ADC1->RSQR3 |= (adc_channel<<5*0);
	
	//---------------------------------------
	// ADCのサンプリング・サイクル設定
	//---------------------------------------
	// 9.3.5 ADC Sample Time Configuration Register 2 (ADC_SAMPTR2)
	// 各チャネルのサンプリング・サイクル数を設定
	// SAMPTR1～SAMPTR2に渡って、0～15個の最大16個のチャンネル毎のサンプリングサイクル数を指定できる
	// チャンネル番号に相当するSMPx(xがチャンネル番号になっている)ビットにサイクル数を指定する
	// 000: 3 cycles; 001: 9 cycles.
	// 010: 15 cycles; 011: 30 cycles.
	// 100: 43 cycles; 101:57 cycles.
	// 110: 73 cycles; 111: 241 cycles.

	// channel0 => SMP0
	// sampling cycle = 241 cycles => 111
	ADC1->SAMPTR2 &= ~(0b111<<(3*adc_channel));
	ADC1->SAMPTR2 |= 0b111<<(3*adc_channel);
		
	//---------------------------------------
	// ADC 変換開始、トリガー設定
	//---------------------------------------
	// 9.3.3 ADC Control Register 2 (ADC_CTLR2)
	// ADC 変換開始イベントは外部イベントによってトリガーする事ができる
	// 以下はSWSTARTを設定することで開始できるようになる
	// ADC_ADON=0x00000001 => A/D Converter Start Conversion
	// ADC_EXTSEL=0x000E0000 => 111(SWSTART software trigger)
	ADC1->CTLR2 |= ADC_ADON | ADC_EXTSEL;
	
	// データのアライメント
	// 10bitのADC結果の値を左揃えまたは右揃えを指定できる
	// ADC_CTLR2 レジスタの ALIGN ビットで指定する
	// デフォルトは0で、right alignmentになり、
	// 右詰めでRDATARに値が格納される(10bit, 0-1023)

	// ADCキャリブレーションのリセット
	ADC1->CTLR2 |= ADC_RSTCAL;
	while(ADC1->CTLR2 & ADC_RSTCAL);
	
	// ADCキャリブレーションの開始
	ADC1->CTLR2 |= ADC_CAL;
	while(ADC1->CTLR2 & ADC_CAL);
	
}

//---------------------------------------
// ADCの開始および値の取得
//---------------------------------------
uint16_t adc_get( void )
{
	// DMAや割込みを使わずにレジスタを直接参照して値を取得する

	// ADCのソフトウェアスタート
	ADC1->CTLR2 |= ADC_SWSTART;
	// ADC変換終了を待つ
	while(!(ADC1->STATR & ADC_EOC));
	// ADC変換結果の取得(10bit, 0-1023)
	return ADC1->RDATAR;
}

int main()
{
	uint32_t count = 0;
	
	SystemInit();

	printf("\r\r\n\nadc_polled example\n\r");

	// init systick @ 1ms rate
	printf("initializing adc...");
	adc_init();
	printf("done.\n\r");
	
	/*
	// Enable GPIOC
	RCC->APB2PCENR |= RCC_APB2Periph_GPIOC;
	// GPIO PC1 Push-Pull
	// 0000000011110000を反転してクリア
	GPIOC->CFGLR &= ~(0xf<<(4*1));
	// 10MHz, Output Push Pull
	GPIOC->CFGLR |= (GPIO_Speed_10MHz | GPIO_CNF_OUT_PP)<<(4*1);
	*/
	printf("looping...\n\r");
	while(1)
	{
		/*
		// LED(PC1)点滅
		GPIOC->BSHR = (1<<1);
		Delay_Ms( 100 );
		GPIOC->BSHR = (1<<(16+1));
		Delay_Ms( 100 );
		*/
		// AD値読み取り(PA2/A0)
		printf( "Count: %lu adc: %d\n\r", count++, adc_get() );
	}
}
