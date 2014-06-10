#include "hw_config.h"
#include "quadrature_encoder.h"

//-------------------------------------------#DEFINE------------------------------------------
#define Codeur_A           GPIO_Pin_8
#define Codeur_A_SOURCE    GPIO_PinSource8
#define Codeur_B           GPIO_Pin_9
#define Codeur_B_SOURCE    GPIO_PinSource9
#define Codeur_GPIO        GPIOA
#define Codeur_RCC         RCC_AHBPeriph_GPIOA
#define Codeur_AF          GPIO_AF_6
 
#define Codeur_TIMER       TIM1

#define BUTTON_PIN 			GPIO_Pin_8 
#define BUTTON_GPIO			GPIOC

void QuadEncInit()
{
    RCC_AHBPeriphClockCmd(Codeur_RCC, ENABLE);
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
 
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
 
    TIM_TimeBaseStructure.TIM_Prescaler = 0;
    TIM_TimeBaseStructure.TIM_Period = 65535; // Maximal
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
 
    TIM_TimeBaseInit(Codeur_TIMER, &TIM_TimeBaseStructure);
 
    // TIM_EncoderMode_TI1: Counter counts on TI1FP1 edge depending on TI2FP2 level.
    TIM_EncoderInterfaceConfig(Codeur_TIMER, TIM_EncoderMode_TI1, TIM_ICPolarity_Rising, TIM_ICPolarity_Rising);
     
    TIM_Cmd(Codeur_TIMER, ENABLE);   

    //button PC8
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOC, ENABLE);
 
    GPIO_InitStructure.GPIO_Pin = BUTTON_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(BUTTON_GPIO, &GPIO_InitStructure);
}

uint16_t QuadEncValue()
{
	return TIM_GetCounter (Codeur_TIMER) ;
}

bool QuadEncButton()
{
	return GPIO_ReadInputDataBit(BUTTON_GPIO, BUTTON_PIN)==0;
}