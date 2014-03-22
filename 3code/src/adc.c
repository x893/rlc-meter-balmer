// balmer@inbox.ru 2013 RLC Meter
#include "hw_config.h"
#include <math.h>
#include "adc.h"
#include "dac.h"
#include "voltage.h"
#include "usb_desc.h"
#include "systick.h"

#include "lcd_interface.h"//test code


/*
	g_resultBuffer организован не очень удобно.
	В нем хранятся uint16_t данные.
	Вначале идет g_ResultBufferSize сэмплов от V канала.
	Потом идет g_ResultBufferSize сэмплов от I канала, начиная
	со смещения g_resultBuffer[g_ResultBufferSize/2].

	g_resultBuffer - данные непосредственно от ADC, постоянно пишутся в циклическом режиме.
	g_resultBufferCopy - копия данных, пишется только по требованию и не обновляется после этого.
*/
static uint32_t g_resultBuffer[RESULT_BUFFER_SIZE];
uint32_t g_resultBufferCopy[RESULT_BUFFER_SIZE];
uint32_t g_ResultBufferSize = RESULT_BUFFER_SIZE;
static volatile uint8_t g_adc_cycles;
static volatile uint8_t g_cur_cycle;

static volatile bool g_usb_request_data = false; //Данные затребованы для получения
static volatile bool g_usb_sampled_data = false; //Данные отсэмплированны и помещенны в буфер g_resultBufferCopy

uint16_t g_adcStatus = 0;
uint16_t g_adc_cur_read_pos;
bool g_adc_read_buffer = false;
uint32_t g_adc_elapsed_time = 0;

AdcSummaryData g_data;

void AdcRoundSize(uint32_t dac_samples_per_period)
{
	//требуется g_ResultBufferSize%dac_samples_per_period==0
	g_ResultBufferSize = (RESULT_BUFFER_SIZE/dac_samples_per_period)*dac_samples_per_period;
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
    DMA_InitStructure.DMA_BufferSize = g_ResultBufferSize;
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
    DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)&g_resultBuffer[g_ResultBufferSize/2];
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
    DMA_InitStructure.DMA_BufferSize = g_ResultBufferSize;
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

void AdcUsbStartReadBuffer()
{	
	g_adc_cur_read_pos = 0;
	g_adc_read_buffer = true;
	USBAdd32(g_ResultBufferSize);
	USBAdd32(g_adc_elapsed_time);
	USBAdd32(g_adc_cycles);
}

static void AdcReadBufferComplete()
{
	g_usb_sampled_data = false;
	g_adc_read_buffer = false;
}

bool AdcUsbBufferComplete()
{
	return g_usb_sampled_data;
}

void AdcUsbRequestData()
{
	g_usb_request_data = true;
	g_usb_sampled_data = false;
}

void AdcUsbReadBuffer()
{
	if(g_adc_cur_read_pos>=g_ResultBufferSize)
	{
		AdcReadBufferComplete();
		return;
	}

	uint32_t* buffer = g_resultBufferCopy;

	uint32_t sz = g_ResultBufferSize - g_adc_cur_read_pos;
	//const uint32_t max_elements = VIRTUAL_COM_PORT_DATA_SIZE/sizeof(buffer[0]);
	const uint32_t max_elements = VIRTUAL_COM_PORT_DATA_SIZE/sizeof(buffer[0])/2; //BUGFIX Чтото не понимаю в USB

	uint32_t to_send = sz>max_elements?max_elements:sz;

	USBAdd((uint8_t*)(buffer+g_adc_cur_read_pos), to_send*sizeof(buffer[0]));
	g_adc_cur_read_pos+=to_send;

	USBSend();

	if(g_adc_cur_read_pos>=g_ResultBufferSize)
	{
		AdcReadBufferComplete();
		return;
	}
}


void AdcDacStartSynchro(uint32_t period, uint16_t amplitude)
{
	if(g_adcStatus==1)
		AdcStop();//Потенциально здесь может все зависнуть, если цикл внутри AdcQuant не завершился
	//for(int i=0;i<RESULT_BUFFER_SIZE;i++)
	//	g_resultBuffer[i]=0x00080008;

	g_cur_cycle = 0;

	LcdFrequency(SystemCoreClock/period);
	LcdRepaint();

	DacSetPeriod(period, amplitude);
	AdcRoundSize(DacSamplesPerPeriod());
	AdcStartPre();

	USBAdd32(DacPeriod());
	USBAdd32(SystemCoreClock);
	USBAdd32(DacSamplesPerPeriod());

	ADC_StartConversion(ADC3);
	ADC_StartConversion(ADC4);

	g_adc_elapsed_time = 0;
	StartTimer();
	TIM_Cmd(TIM2, ENABLE); //Start DAC
}


static void AdcResultBufferCopy(uint16_t offset, uint16_t count)
{
	uint16_t* inV = offset+(uint16_t*)g_resultBuffer;
	uint16_t* inI = offset+(uint16_t*)&g_resultBuffer[g_ResultBufferSize/2];

	uint16_t* outV = offset+(uint16_t*)g_resultBufferCopy;
	uint16_t* outI = offset+(uint16_t*)&g_resultBufferCopy[g_ResultBufferSize/2];

	for(uint16_t i=0; i<count; i++)
		outV[i] = inV[i];

	for(uint16_t i=0; i<count; i++)
		outI[i] = inI[i];
}

//Когда начинается следующий квант
//Функция должна быстро отрабатывать
static bool AdcOnNextRequest()
{
	return g_usb_request_data;
}

//Когда данные скопированны в g_resultBufferCopy
static void AdcOnComplete()
{
	g_usb_request_data = false;

	uint16_t* inV = (uint16_t*)g_resultBufferCopy;
	uint16_t* inI = (uint16_t*)&g_resultBufferCopy[g_ResultBufferSize/2];

	g_data.count = g_ResultBufferSize;

	AdcAddData(&g_data, inV, inI, g_ResultBufferSize);

	g_usb_sampled_data = true;
}

void AdcQuant()
{
	if(g_adcStatus!=1)
		return;
	//if(g_usb_sampled_data)
	//	return;

	bool isNextQuant = g_cur_cycle!=g_adc_cycles;

	if(!isNextQuant)
		return;

	g_cur_cycle = g_adc_cycles;

	if(DMA2_Channel5->CNDTR < g_ResultBufferSize-(g_ResultBufferSize/4) )
	{
		//Этот квант уже не успеем скопировать, пропускаем
		return;
	}

	if(!AdcOnNextRequest())
		return;

	uint16_t curOffset = 0;


	while(1)
	{
		uint16_t counter = (uint16_t)DMA2_Channel5->CNDTR;//Сколько данных осталось записать
		uint16_t nextOffset = g_ResultBufferSize-counter;
		if(g_cur_cycle!=g_adc_cycles)
			break;

		if(curOffset<nextOffset)
		{
			AdcResultBufferCopy(curOffset, nextOffset-curOffset);
			curOffset = nextOffset;
		}

	}


	if(curOffset<g_ResultBufferSize)
	{
		AdcResultBufferCopy(curOffset, g_ResultBufferSize-curOffset);
	}

	g_cur_cycle++;

	AdcClearData(&g_data);
	if(g_cur_cycle!=g_adc_cycles)
	{
		g_data.error = true;
	}

	g_data.nop_number = DMA2_Channel5->CNDTR;

	AdcOnComplete();

}

void AdcSendLastComputeCh(AdcSummaryChannel* ch)
{
	USBAdd16(ch->adc_min);
	USBAdd16(ch->adc_max);
	USBAddFloat(ch->k_sin);
	USBAddFloat(ch->k_cos);
	USBAddFloat(ch->adc_mid);
	USBAddFloat(ch->square_error);
}

void AdcSendLastCompute()
{
	AdcSummaryData* data = &g_data;
	USBAdd16(data->count);
	AdcSendLastComputeCh(&data->ch_v);
	AdcSendLastComputeCh(&data->ch_i);
	USBAdd8(data->error);
	USBAdd32(data->nop_number);

	g_usb_sampled_data = false;
}
