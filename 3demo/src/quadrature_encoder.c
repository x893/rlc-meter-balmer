#include "hw_config.h"
#include "quadrature_encoder.h"

//-------------------------------------------#DEFINE------------------------------------------
//  |  TIM4_CH1  pin (PB.06) |
//  |  TIM4_CH2  pin (PB.07) |
//  --------------------------
//PINS CODEUR A ET B
#define Codeur_A           GPIO_Pin_8
#define Codeur_A_SOURCE    GPIO_PinSource8
#define Codeur_B           GPIO_Pin_9
#define Codeur_B_SOURCE    GPIO_PinSource9
#define Codeur_GPIO        GPIOA
#define Codeur_RCC         RCC_AHBPeriph_GPIOA
#define Codeur_AF          GPIO_AF_6
 
//TIMER UTILISE
#define Codeur_TIMER       TIM1
#define Codeur_COUNT()     Codeur_TIMER->CNT
 
//LEDS DEMOBOARD
#define Led_Blue           GPIO_Pin_10
#define Led_GPIO           GPIOA
#define Led_RCC            RCC_AHBPeriph_GPIOA

void QuadEncInit()
{
    // Enable RCC codeur
    RCC_AHBPeriphClockCmd(Codeur_RCC, ENABLE);
    // Enable RCC led
    RCC_AHBPeriphClockCmd(Led_RCC, ENABLE);
    // Enable Timer 4 clock
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);

    GPIO_InitTypeDef GPIO_InitStructure;
 
    // 2 Inputs for A and B Encoder Channels
    GPIO_InitStructure.GPIO_Pin = Codeur_A|Codeur_B;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    //GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(Codeur_GPIO, &GPIO_InitStructure);
 
    //Timer AF Pins Configuration
    GPIO_PinAFConfig(Codeur_GPIO, Codeur_A_SOURCE, Codeur_AF);
    GPIO_PinAFConfig(Codeur_GPIO, Codeur_B_SOURCE, Codeur_AF);
 /*
    // Output Leds
    GPIO_InitStructure.GPIO_Pin = Led_Blue;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(Led_GPIO, &GPIO_InitStructure);
*/

    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
 
    TIM_TimeBaseStructure.TIM_Prescaler = 0;
    TIM_TimeBaseStructure.TIM_Period = 65535; // Maximal
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
 
    TIM_TimeBaseInit(Codeur_TIMER, &TIM_TimeBaseStructure);
 
    // TIM_EncoderMode_TI1: Counter counts on TI1FP1 edge depending on TI2FP2 level.
    TIM_EncoderInterfaceConfig(Codeur_TIMER, TIM_EncoderMode_TI1, TIM_ICPolarity_Rising, TIM_ICPolarity_Rising);
    //TIM_EncoderInterfaceConfig(Codeur_TIMER, TIM_EncoderMode_TI2, TIM_ICPolarity_Falling, TIM_ICPolarity_Falling);
     
    TIM_Cmd(Codeur_TIMER, ENABLE);   
/*     
	// Init timer
    // set them up as encoder inputs
    // set both inputs to rising polarity to let it use both edges
    TIM_EncoderInterfaceConfig (Codeur_TIMER, TIM_EncoderMode_TI12, TIM_ICPolarity_Rising, TIM_ICPolarity_Rising);
    TIM_SetAutoreload (Codeur_TIMER, 0xffff); 
    TIM_SetCounter (Codeur_TIMER, 0);
    // turn on the timer/counters
    TIM_Cmd (Codeur_TIMER, ENABLE);
*/
}

uint16_t QuadEncValue()
{
	return TIM_GetCounter (Codeur_TIMER) ;
}

void QuadEncInit2()
{
    // Enable RCC codeur
    RCC_AHBPeriphClockCmd(Codeur_RCC, ENABLE);
    GPIO_InitTypeDef GPIO_InitStructure;
 
    // 2 Inputs for A and B Encoder Channels
    GPIO_InitStructure.GPIO_Pin = Codeur_A|Codeur_B;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(Codeur_GPIO, &GPIO_InitStructure);
}

uint8_t QuadEncValueA()
{
	return GPIO_ReadInputDataBit(Codeur_GPIO, Codeur_A);
}

uint8_t QuadEncValueB()
{
	return GPIO_ReadInputDataBit(Codeur_GPIO, Codeur_B);
}