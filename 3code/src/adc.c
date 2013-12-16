#include "hw_config.h"
#include <math.h>
#include "adc.h"
#include "dac.h"
#include "voltage.h"
#include "usb_desc.h"
#include "systick.h"

#define RESULT_BUFFER_SIZE 1000

static uint32_t g_resultBuffer[RESULT_BUFFER_SIZE];
static uint32_t ResultBufferSize = RESULT_BUFFER_SIZE;
static uint8_t g_adc_cycles;
static uint8_t g_adc_cycles_skip = 0;

uint16_t g_adcStatus = 0;
uint16_t g_adc_cur_read_pos;
bool g_adc_read_buffer = false;
uint32_t g_adc_elapsed_time = 0;

void DMA1_Channel1_IRQHandler(void)
{
	if(DMA_GetITStatus(DMA1_IT_TC1) == SET)
	{
		DMA_ClearITPendingBit(DMA1_IT_GL1);

		if(g_adc_cycles++<g_adc_cycles_skip)//skip first read
			return;

		//g_adc_elapsed_time = GetTime();
		g_adcStatus = 2;

		ADC_DMACmd(ADC1, DISABLE);
		ADC_Cmd(ADC1, DISABLE);
		ADC_Cmd(ADC2, DISABLE);
	}
}

static void NVIC_Configuration(void)
{
        NVIC_InitTypeDef NVIC_InitStructure;

        NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);

        NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel1_IRQn;
        NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
        NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
        NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
        NVIC_Init(&NVIC_InitStructure);
}

void AdcInit()
{
	RCC_ADCCLKConfig(RCC_ADC12PLLCLK_Div1);

	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOC, ENABLE);
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_ADC12, ENABLE);

	NVIC_Configuration();

	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOC, ENABLE);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 ;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	ADC_VoltageRegulatorCmd(ADC1, ENABLE);
	delay_us(20);

	ADC_SelectCalibrationMode(ADC1, ADC_CalibrationMode_Single);
	ADC_StartCalibration(ADC1);
	while(ADC_GetCalibrationStatus(ADC1) != RESET );

	ADC_SelectCalibrationMode(ADC2, ADC_CalibrationMode_Single);
	ADC_StartCalibration(ADC2);
	while(ADC_GetCalibrationStatus(ADC2) != RESET );

	ADC_CommonInitTypeDef ADC_CommonInitStructure;
	ADC_CommonInitStructure.ADC_Mode = ADC_Mode_RegSimul;                                                                    
	ADC_CommonInitStructure.ADC_Clock = ADC_Clock_AsynClkMode;                    
	ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_1;             
	ADC_CommonInitStructure.ADC_DMAMode = ADC_DMAMode_OneShot;                  
	ADC_CommonInitStructure.ADC_TwoSamplingDelay = 0;          
	ADC_CommonInit(ADC1, &ADC_CommonInitStructure);

	ADC_InitTypeDef ADC_InitStructure;

	ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;
	if(1)
	{
		ADC_InitStructure.ADC_ContinuousConvMode = ADC_ContinuousConvMode_Disable;
		ADC_InitStructure.ADC_ExternalTrigConvEvent = ADC_ExternalTrigConvEvent_3;
		ADC_InitStructure.ADC_ExternalTrigEventEdge = ADC_ExternalTrigEventEdge_RisingEdge;
	} else
	{
		ADC_InitStructure.ADC_ContinuousConvMode = ADC_ContinuousConvMode_Enable;
		ADC_InitStructure.ADC_ExternalTrigConvEvent = ADC_ExternalTrigConvEvent_0;
		ADC_InitStructure.ADC_ExternalTrigEventEdge = ADC_ExternalTrigEventEdge_None;
	}

	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
	ADC_InitStructure.ADC_OverrunMode = ADC_OverrunMode_Disable;
	ADC_InitStructure.ADC_AutoInjMode = ADC_AutoInjec_Disable;
	ADC_InitStructure.ADC_NbrOfRegChannel = 1;
	ADC_Init(ADC1, &ADC_InitStructure);
	ADC_Init(ADC2, &ADC_InitStructure);

	//ADC_RegularChannelConfig(ADC1, ADC_Channel_7/*PC1*/, 1, ADC_SampleTime_7Cycles5);
	//ADC_RegularChannelConfig(ADC2, ADC_Channel_6/*PC0*/, 1, ADC_SampleTime_7Cycles5);
	//ADC_RegularChannelConfig(ADC1, ADC_Channel_7/*PC1*/, 1, ADC_SampleTime_19Cycles5);
	//ADC_RegularChannelConfig(ADC2, ADC_Channel_6/*PC0*/, 1, ADC_SampleTime_19Cycles5);
}

void AdcRoundSize(uint32_t dac_samples_per_period)
{
	//требуется ResultBufferSize%dac_samples_per_period==0
	ResultBufferSize = (RESULT_BUFFER_SIZE/dac_samples_per_period)*dac_samples_per_period;
}

void AdcStartPre()
{
	DMA_InitTypeDef DMA_InitStructure;
	DMA_DeInit(DMA1_Channel1);
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&ADC1_2->CDR;
    DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)&g_resultBuffer[0];
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
    DMA_InitStructure.DMA_BufferSize = ResultBufferSize;
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Word;
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Word;
    DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
    DMA_InitStructure.DMA_Priority = DMA_Priority_High;
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
    DMA_Init(DMA1_Channel1, &DMA_InitStructure);
    DMA_Cmd(DMA1_Channel1, ENABLE);
    DMA_ITConfig(DMA1_Channel1, DMA_IT_TC, ENABLE);

	DMA_SetCurrDataCounter(DMA1_Channel1, 0);

	uint8_t sample_ticks = DacSampleTicks()<72?ADC_SampleTime_7Cycles5:ADC_SampleTime_19Cycles5;
	ADC_RegularChannelConfig(ADC1, ADC_Channel_7/*PC1*/, 1, sample_ticks);
	ADC_RegularChannelConfig(ADC2, ADC_Channel_6/*PC0*/, 1, sample_ticks);

	ADC_Cmd(ADC1, ENABLE);
	ADC_Cmd(ADC2, ENABLE);
	ADC_DMACmd(ADC1, ENABLE);
	g_adcStatus = 1;
	g_adc_cycles = 0;
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

void AdcDacStartSynchro(uint32_t period, uint8_t num_skip)
{
	g_adc_cycles_skip = num_skip;
	DacSetPeriod(period);
	AdcRoundSize(DacSamplesPerPeriod());
	AdcStartPre();

	USBAdd32(DacPeriod());
	USBAdd32(SystemCoreClock);
	USBAdd32(DacSamplesPerPeriod());

	ADC_StartConversion(ADC1);
	TIM_Cmd(TIM2, ENABLE); //Start DAC
}
