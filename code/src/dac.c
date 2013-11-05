#include "hw_config.h"
#include <math.h>
#include "SysTick/systick.h"

#define pi  3.14159f
#define SINUS_BUFFER_SIZE 1000
#define DAC_ZERO 2047
#define DAC_AMPLITUDE 1000
#define MAX_FREQUENCY 200000


static uint16_t g_sinusBuffer[SINUS_BUFFER_SIZE];
static uint32_t SinusBufferSize = SINUS_BUFFER_SIZE;
static uint32_t g_period = 0; // * 1/SystemCoreClock sec SystemCoreClock==72000000

uint32_t DacPeriod(void)
{
	return g_period;
}

void DacSinusCalculate()
{
	float mul = 2*pi/SinusBufferSize;
	for(int i=0; i<SinusBufferSize; i++)
	{
		g_sinusBuffer[i] = (uint16_t) lround(sin(i*mul)*DAC_AMPLITUDE)+DAC_ZERO;
	}
}

void DacInit(void)
{  
	DacSinusCalculate();
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_DAC, ENABLE);
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_DAC, ENABLE);	

	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_4;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

  	DAC_InitTypeDef DAC_InitStructure;
	DAC_InitStructure.DAC_Trigger = DAC_Trigger_None;
	DAC_InitStructure.DAC_WaveGeneration = DAC_WaveGeneration_None;
	DAC_InitStructure.DAC_OutputBuffer = DAC_OutputBuffer_Enable;
	
	DAC_Init(DAC_Channel_1, &DAC_InitStructure);
	delay_us(100);
	DAC_Cmd(DAC_Channel_1, ENABLE);
	DAC_SetChannel1Data(DAC_Align_12b_R, DAC_ZERO);

}

/*
If frequency<=1 khz
	SinusBufferSize maximal
	TIM_Period = 72000000 / SINUS_BUFFER_SIZE / frequency
*/
void DacSetFrequency(uint32_t frequency)
{
	if(frequency>MAX_FREQUENCY)
		frequency = MAX_FREQUENCY;
	//assert_param(frequency>=0 && frequency<=300000);
	DMA_Cmd(DMA1_Channel2, DISABLE);
	TIM_Cmd(TIM2, DISABLE);
	if(frequency==0)
	{
		g_period = 0xFFFFFFFF;
		DAC_SetChannel1Data(DAC_Align_12b_R, DAC_ZERO);
		return;
	}

	DAC_SetChannel1Data(DAC_Align_12b_R, DAC_ZERO);

	uint32_t prescaler;
	uint32_t period;
	if(frequency<=2400)
	{
		SinusBufferSize = SINUS_BUFFER_SIZE;
		uint32_t p = SystemCoreClock/SINUS_BUFFER_SIZE/frequency;
		prescaler = 1;
		period = 1;
		while(p>0xFFFFul*prescaler)
		{
			prescaler++;
		}

		period = p / prescaler;
	} else
	{
		prescaler = 1;
		period = 30;
		SinusBufferSize = SystemCoreClock/period/frequency;
	}

	DacSinusCalculate();

	DMA_InitTypeDef DMA_InitStructure;
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&DAC->DHR12R1;
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)&g_sinusBuffer;
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
	DMA_InitStructure.DMA_BufferSize = SinusBufferSize;
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
	DMA_Init(DMA1_Channel2, &DMA_InitStructure);
	DMA_Cmd(DMA1_Channel2, ENABLE);


	//72 MHz / TIM_Prescaler / TIM_Period
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);
	TIM_TimeBaseStructure.TIM_Period = period-1;
	TIM_TimeBaseStructure.TIM_Prescaler = prescaler-1;
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);

	TIM_SelectOutputTrigger(TIM2, TIM_TRGOSource_Update);
	TIM_DMACmd(TIM2, TIM_DMA_Update, ENABLE);
	TIM_Cmd(TIM2, ENABLE);

	g_period = period * prescaler * SinusBufferSize;
	//g_period = frequency;
}