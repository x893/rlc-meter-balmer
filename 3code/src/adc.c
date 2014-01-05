// balmer@inbox.ru 2013 RLC Meter
#include "hw_config.h"
#include <math.h>
#include "adc.h"
#include "dac.h"
#include "voltage.h"
#include "usb_desc.h"
#include "systick.h"

//При размере RESULT_BUFFER_SIZE == 2000 начинает виснуть на низких частотах
//Не передается последний пакет.
#define RESULT_BUFFER_SIZE 3000

static uint32_t g_resultBuffer[RESULT_BUFFER_SIZE];
static uint32_t ResultBufferSize = RESULT_BUFFER_SIZE;
static uint8_t g_adc_cycles;
static uint8_t g_adc_cycles_skip = 0;

uint16_t g_adcStatus = 0;
uint16_t g_adc_cur_read_pos;
bool g_adc_read_buffer = false;
uint32_t g_adc_elapsed_time = 0;

static uint8_t g_cur_cycle;
static uint32_t g_averageBuffer[RESULT_BUFFER_SIZE];
static uint16_t g_averageCount;
static uint16_t g_averageNumber = 1; //max average count
static uint16_t g_averageError;


void AdcRoundSize(uint32_t dac_samples_per_period)
{
	//требуется ResultBufferSize%dac_samples_per_period==0
	ResultBufferSize = (RESULT_BUFFER_SIZE/dac_samples_per_period)*dac_samples_per_period;
}

void AdcStop()
{
	StopTimer();
	g_adc_elapsed_time = GetTime();
	g_adcStatus = 2;

	ADC_StopConversion(ADC3);
	ADC_StopConversion(ADC4);
}

//for ADC34
void DMA2_Channel5_IRQHandler(void)
{
	if(DMA2->ISR & DMA_ISR_TCIF5)//transfer complete
	{
		g_adc_cycles++;
	/*
		if(g_adc_cycles++>=g_adc_cycles_skip)
		{
			AdcStop();
		}
	*/
	}

	DMA2->IFCR = DMA_IFCR_CGIF5;
}

//////////////

static void NVIC_Configuration34(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;

    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);

    NVIC_InitStructure.NVIC_IRQChannel = DMA2_Channel5_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

static void AdcInit34()
{
	RCC_ADCCLKConfig(RCC_ADC34PLLCLK_Div1);

	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA2, ENABLE);
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_ADC34, ENABLE);

	NVIC_Configuration34();

	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1 ;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOE, ENABLE);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14 ;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;
	GPIO_Init(GPIOE, &GPIO_InitStructure);

	ADC_VoltageRegulatorCmd(ADC3, ENABLE);
	ADC_VoltageRegulatorCmd(ADC4, ENABLE);
	delay_us(20);

	ADC_SelectCalibrationMode(ADC3, ADC_CalibrationMode_Single);
	ADC_StartCalibration(ADC3);
	while(ADC_GetCalibrationStatus(ADC3) != RESET );

	ADC_SelectCalibrationMode(ADC4, ADC_CalibrationMode_Single);
	ADC_StartCalibration(ADC4);
	while(ADC_GetCalibrationStatus(ADC4) != RESET );

	ADC_CommonInitTypeDef ADC_CommonInitStructure;
	ADC_CommonInitStructure.ADC_Mode = ADC_Mode_RegSimul;
	//ADC_CommonInitStructure.ADC_Mode = ADC_Mode_Independent;
	ADC_CommonInitStructure.ADC_Clock = ADC_Clock_SynClkModeDiv1;
	//ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_1;
	ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled;
	//ADC_CommonInitStructure.ADC_DMAMode = ADC_DMAMode_OneShot;
	ADC_CommonInitStructure.ADC_DMAMode = ADC_DMAMode_Circular;
	ADC_CommonInitStructure.ADC_TwoSamplingDelay = 0;          
	ADC_CommonInit(ADC3, &ADC_CommonInitStructure);

	ADC_InitTypeDef ADC_InitStructure;

	ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;
	if(1)
	{
		ADC_InitStructure.ADC_ContinuousConvMode = ADC_ContinuousConvMode_Disable;
		ADC_InitStructure.ADC_ExternalTrigConvEvent = ADC_ExternalTrigConvEvent_1;
		ADC_InitStructure.ADC_ExternalTrigEventEdge = ADC_ExternalTrigEventEdge_RisingEdge;
	} else
	{
		ADC_InitStructure.ADC_ContinuousConvMode = ADC_ContinuousConvMode_Enable;
		ADC_InitStructure.ADC_ExternalTrigConvEvent = ADC_ExternalTrigConvEvent_0;
		ADC_InitStructure.ADC_ExternalTrigEventEdge = ADC_ExternalTrigEventEdge_None;
	}

	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
	ADC_InitStructure.ADC_OverrunMode = ADC_OverrunMode_Disable;
	//ADC_InitStructure.ADC_OverrunMode = ADC_OverrunMode_Enable;
	ADC_InitStructure.ADC_AutoInjMode = ADC_AutoInjec_Disable;
	ADC_InitStructure.ADC_NbrOfRegChannel = 1;
	ADC_Init(ADC3, &ADC_InitStructure);

	ADC_InitStructure.ADC_ExternalTrigEventEdge = ADC_ExternalTrigEventEdge_None;

	ADC_Init(ADC4, &ADC_InitStructure);

	ADC_DMAConfig(ADC3, ADC_DMAMode_Circular);
	ADC_DMAConfig(ADC4, ADC_DMAMode_Circular);
}

static void AdcStartPre34()
{
	DMA_InitTypeDef DMA_InitStructure;
	DMA_DeInit(DMA2_Channel5);
	DMA_DeInit(DMA2_Channel2);

    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&ADC3->DR;
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

    DMA_Init(DMA2_Channel5, &DMA_InitStructure);
    DMA_Cmd(DMA2_Channel5, ENABLE);
    DMA_ITConfig(DMA2_Channel5, DMA_IT_TC, ENABLE);
	DMA_SetCurrDataCounter(DMA2_Channel5, 0);

    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&ADC4->DR;
    DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)&g_resultBuffer[ResultBufferSize/2];
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
    DMA_InitStructure.DMA_BufferSize = ResultBufferSize;
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
    DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
    DMA_InitStructure.DMA_Priority = DMA_Priority_High;
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;

    DMA_Init(DMA2_Channel2, &DMA_InitStructure);
    DMA_Cmd(DMA2_Channel2, ENABLE);
    DMA_ITConfig(DMA2_Channel2, DMA_IT_TC, ENABLE);
	DMA_SetCurrDataCounter(DMA2_Channel2, 0);

	uint8_t sample_ticks = DacSampleTicks()<72?ADC_SampleTime_7Cycles5:ADC_SampleTime_19Cycles5;
	ADC_RegularChannelConfig(ADC3, ADC_Channel_1/*PB1*/, 1, sample_ticks);
	ADC_RegularChannelConfig(ADC4, ADC_Channel_1/*PE14*/, 1, sample_ticks);

	ADC_Cmd(ADC3, ENABLE);
	ADC_Cmd(ADC4, ENABLE);
	ADC_DMACmd(ADC3, ENABLE);
	ADC_DMACmd(ADC4, ENABLE);
	g_adcStatus = 1;
	g_adc_cycles = 0;
}

void AdcInit()
{
	AdcInit34();
}

void AdcStartPre()
{
	AdcStartPre34();
}

static void AdcInitAverage(uint16_t average_number)
{
	g_averageCount = 0;
	g_averageNumber = average_number;
	for(int i=0;i<RESULT_BUFFER_SIZE;i++)
		g_averageBuffer[i] = 0;
	g_cur_cycle = 0;
	g_averageError++;
}

static void AdcCalcAverage()
{
	if(g_averageCount==0)
		return;
	uint16_t* outV = (uint16_t*)g_averageBuffer;
	uint16_t* outI = (uint16_t*)&g_averageBuffer[ResultBufferSize/2];

	uint16_t a2 = g_averageCount/2;

	for(int i=0;i<ResultBufferSize;i++)
	{
		outV[i] = (outV[i]+a2)/g_averageCount;
		outI[i] = (outI[i]+a2)/g_averageCount;
	}
}


void AdcStartReadBuffer()
{	
	AdcCalcAverage();
	g_adc_cur_read_pos = 0;
	g_adc_read_buffer = true;
	USBAdd32(ResultBufferSize);
	USBAdd32(g_adc_elapsed_time);
	USBAdd32(g_adc_cycles);
	USBAdd8(g_averageCount);
}

void AdcReadBuffer()
{
	if(g_adc_cur_read_pos>=ResultBufferSize)
	{
		g_adc_read_buffer = false;
		return;
	}

	//uint32_t* buffer = g_resultBuffer;
	uint32_t* buffer = g_averageBuffer;

	uint32_t sz = ResultBufferSize - g_adc_cur_read_pos;
	const uint32_t max_elements = VIRTUAL_COM_PORT_DATA_SIZE/sizeof(buffer[0]);
	if(sz>max_elements)
	{
		USBAdd((uint8_t*)(buffer+g_adc_cur_read_pos), max_elements*sizeof(buffer[0]));
		g_adc_cur_read_pos+=max_elements;
	}
	else
	{
		USBAdd((uint8_t*)(buffer+g_adc_cur_read_pos), sz*sizeof(buffer[0]));
		g_adc_cur_read_pos+=sz;
		g_adc_read_buffer = false;
	}

	USBSend();
}


void AdcDacStartSynchro(uint32_t period, uint8_t num_skip, uint16_t average_number)
{
	for(int i=0;i<RESULT_BUFFER_SIZE;i++)
		g_resultBuffer[i]=0x00080008;

	AdcInitAverage(average_number);

	g_adc_cycles_skip = num_skip;
	DacSetPeriod(period);
	AdcRoundSize(DacSamplesPerPeriod());
	AdcStartPre();

	USBAdd32(DacPeriod());
	USBAdd32(SystemCoreClock);
	USBAdd32(DacSamplesPerPeriod());
	USBAdd8(g_adc_cycles_skip);

	ADC_StartConversion(ADC3);
	ADC_StartConversion(ADC4);

	g_adc_elapsed_time = 0;
	StartTimer();
	TIM_Cmd(TIM2, ENABLE); //Start DAC
}

void AddArray(uint16_t* pout, uint16_t* pin, uint16_t count)
{
	for(uint16_t i=0; i<count; i++)
	{
		pout[i] +=  pin[i];
	}
}

void AdcQuant()
{
	if(g_adcStatus!=1)
		return;

	if(g_adc_cycles<g_adc_cycles_skip)
	{
		g_cur_cycle++;
		return;
	}
/*
	Обрабатываем данные, приходящие с ADC "на лету".
	После обработки одного цикла данных передаем его для хранения в другую функцию.
	Если по какимто причинам не успеваем обработать текущий frame, то ждем, пока он закончится и начинаем обработку заново.
*/
	uint16_t* inV = (uint16_t*)g_resultBuffer;
	uint16_t* inI = (uint16_t*)&g_resultBuffer[ResultBufferSize/2];

	uint16_t* outV = (uint16_t*)g_averageBuffer;
	uint16_t* outI = (uint16_t*)&g_averageBuffer[ResultBufferSize/2];

	uint16_t curOffset = 0;

	while(1)
	{
		uint16_t counter = DMA_GetCurrDataCounter(DMA2_Channel5);//Сколько данных осталось записать
		uint16_t nextOffset = ResultBufferSize-counter;
		if(g_cur_cycle!=g_adc_cycles)
			break;

		if(curOffset<nextOffset)
		{
			AddArray(outV+curOffset, inV+curOffset, nextOffset-curOffset);
			AddArray(outI+curOffset, inI+curOffset, nextOffset-curOffset);
			curOffset = nextOffset;
		}

	}

	if(curOffset<ResultBufferSize)
	{
		AddArray(outV+curOffset, inV+curOffset, ResultBufferSize-curOffset);
		AddArray(outI+curOffset, inI+curOffset, ResultBufferSize-curOffset);
	}

	g_cur_cycle++;
	g_averageCount++;

	if(g_cur_cycle!=g_adc_cycles)
		g_averageError++;

	if(g_averageCount>=g_averageNumber)
	{
		AdcStop();
	}
}
