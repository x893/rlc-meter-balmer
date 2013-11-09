#include "hw_config.h"
#include <math.h>
#include "SysTick/systick.h"
#include "adc.h"
#include "dac.h"
#include "voltage.h"
#include "usb_desc.h"

#define RESULT_BUFFER_SIZE 1000

static uint16_t g_resultBuffer[RESULT_BUFFER_SIZE];
static uint32_t ResultBufferSize = RESULT_BUFFER_SIZE;
static uint8_t g_adc_cycles;
static uint8_t g_adc_cycles_skip = 0;

uint16_t g_adcStatus = 0;
uint16_t g_adc_cur_read_pos;
bool g_adc_read_buffer = false;
uint32_t g_adc_elapsed_time = 0;
uint32_t g_adc_tick = 1;

static void NVIC_Configuration(void)
{
        NVIC_InitTypeDef NVIC_InitStructure;
        //EXTI_InitTypeDef EXTI_InitStructure;

        NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);

        NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel1_IRQn;
        NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
        NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
        NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
        NVIC_Init(&NVIC_InitStructure);
/*
        NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
        NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
        NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
        NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
        NVIC_Init(&NVIC_InitStructure);

        GPIO_EXTILineConfig(GPIO_PortSourceGPIOD, GPIO_PinSource11);
        EXTI_InitStructure.EXTI_Line = EXTI_Line11;
        EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Event;
        EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
        EXTI_InitStructure.EXTI_LineCmd = ENABLE;
        EXTI_Init(&EXTI_InitStructure);
*/
}

void AdcInit()
{
	RCC_ADCCLKConfig(RCC_PCLK2_Div6);//72/6 = 12 MHz

	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC2, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);

	NVIC_Configuration();

	//insert NVIC DMA1_Channel1_IRQn priority here 

	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_Init(GPIOC, &GPIO_InitStructure);   

	ADC_InitTypeDef ADC_InitStructure;
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;//ADC_Mode_RegSimult to dual mode
	//ADC_InitStructure.ADC_ScanConvMode = DISABLE;
	//ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;
	//ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T3_TRGO;
	ADC_InitStructure.ADC_ScanConvMode = ENABLE;
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
	ADC_InitStructure.ADC_NbrOfChannel = 1;
	ADC_Init(ADC1, &ADC_InitStructure);

	ADC_RegularChannelConfig(ADC1, ADC_Channel_10/*PC0*/, 1, ADC_SampleTime_7Cycles5); // 7.5+12.5 clock = 1.66 us time on 12 mhz
	g_adc_tick = 20*6;
	ADC_Cmd(ADC1, ENABLE);


	// Enable ADC1 reset calibaration register
	ADC_ResetCalibration(ADC1);
	// Check the end of ADC1 reset calibration register
	while(ADC_GetResetCalibrationStatus(ADC1)); // Start ADC1 calibaration
	ADC_StartCalibration(ADC1); // Check the end of ADC1 calibration
	while(ADC_GetCalibrationStatus(ADC1));

	//600 khz?
	uint32_t prescaler = 1;
	uint32_t period = 120;
	//72 MHz / TIM_Prescaler / TIM_Period
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);
	TIM_TimeBaseStructure.TIM_Period = period-1;
	TIM_TimeBaseStructure.TIM_Prescaler = prescaler-1;
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);

	TIM_SelectOutputTrigger(TIM3, TIM_TRGOSource_Update);
}

void DMA1_Channel1_IRQHandler(void)
{
	if(DMA_GetITStatus(DMA1_IT_TC1) == SET)
	{
		DMA_ClearITPendingBit(DMA1_IT_GL1);

		if(g_adc_cycles++<g_adc_cycles_skip)//skip first read
			return;

		//g_adc_elapsed_time = GetTime();
		USB_SetLeds('D');
		g_adcStatus = 2;

		ADC_DMACmd(ADC1, DISABLE);
		ADC_Cmd(ADC1, DISABLE);
	}
}

void AdcRoundSize(uint32_t dac_period)
{
	//требуется dac_period%g_adc_tick==0
	uint32_t adc_period = dac_period/g_adc_tick;
	ResultBufferSize = (RESULT_BUFFER_SIZE/adc_period)*adc_period;
}

void AdcStartPre()
{
	USB_SetLeds('C');

	DMA_InitTypeDef DMA_InitStructure;
	DMA_DeInit(DMA1_Channel1);
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&ADC1->DR;
    DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)&g_resultBuffer[0];
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
    DMA_InitStructure.DMA_BufferSize = ResultBufferSize;
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
    DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
    DMA_InitStructure.DMA_Priority = DMA_Priority_High;
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
    DMA_Init(DMA1_Channel1, &DMA_InitStructure);
    DMA_Cmd(DMA1_Channel1, ENABLE);
    DMA_ITConfig(DMA1_Channel1, DMA_IT_TC, ENABLE);
    ADC_DMACmd(ADC1, ENABLE);

	DMA_SetCurrDataCounter(DMA1_Channel1, 0);
	TIM_SetCounter(TIM3, 0);
	ADC_Cmd(ADC1, ENABLE);
	ADC_DMACmd(ADC1, ENABLE);
	g_adcStatus = 1;
	g_adc_cycles = 0;
}

void AdcStart()
{
	AdcStartPre();
	//ADC_ExternalTrigConvCmd(ADC1, ENABLE);
	//TIM_Cmd(TIM3, ENABLE);
	StartTimer();
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);
}

void AdcStartReadBuffer()
{
	g_adc_cur_read_pos = 0;
	g_adc_read_buffer = true;
	USBAdd32(ResultBufferSize);
}

void AdcReadBuffer()
{
	if(g_adc_cur_read_pos>=ResultBufferSize)
	{
		g_adc_read_buffer = false;
		return;
	}

	uint32_t sz = ResultBufferSize - g_adc_cur_read_pos;
	const uint32_t max_elements = VIRTUAL_COM_PORT_DATA_SIZE/sizeof(g_resultBuffer[0]);
	if(sz>max_elements)
	{
		USBAdd((uint8_t*)(g_resultBuffer+g_adc_cur_read_pos), max_elements*sizeof(g_resultBuffer[0]));
		g_adc_cur_read_pos+=max_elements;
	}
	else
	{
		USBAdd((uint8_t*)(g_resultBuffer+g_adc_cur_read_pos), sz*sizeof(g_resultBuffer[0]));
		g_adc_read_buffer = false;
	}

	USBSend();
}

void AdcDacStartSynchro(uint32_t frequency, uint8_t num_skip)
{
	g_adc_cycles_skip = num_skip;
	DacSetFrequency(frequency);
	AdcRoundSize(DacPeriod());
	AdcStartPre();

	USBAdd32(DacPeriod());
	USBAdd32(SystemCoreClock);
	USBAdd32(g_adc_tick);

	if(0)
	{
		TIM_Cmd(TIM2, ENABLE); //Start DAC
		ADC_SoftwareStartConvCmd(ADC1, ENABLE); //Start ADC
	} else
	{
		TIM2->CR1 |= TIM_CR1_CEN; //Start DAC
		ADC1->CR2 |= ADC_CR2_SWSTART|ADC_CR2_EXTTRIG; ////Start ADC
	}
}
