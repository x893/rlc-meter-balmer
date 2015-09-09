// balmer@inbox.ru RLC Meter 303
// 2013-2014

#include "hw_config.h"
#include <math.h>
#include "adc.h"
#include "dac.h"
#include "usb_commands.h"
#include "usb_desc.h"
#include "systick.h"
#include "calc_rc.h"

#include "lcd_interface.h"

/*
	g_resultBuffer организован не очень удобно.
	В нем хранятся uint16_t данные.
	Вначале идет g_ResultBufferSize сэмплов от V канала.
	Потом идет g_ResultBufferSize сэмплов от I канала, начиная
	со смещения g_resultBuffer[g_ResultBufferSize/2].

	g_resultBuffer - данные непосредственно от ADC, постоянно пишутся в циклическом режиме.
	g_resultBufferCopy - копия данных, пишется только по требованию и не обновляется после этого.
	*/

uint32_t g_resultBuffer[RESULT_BUFFER_SIZE];
uint32_t g_resultBufferCopy[RESULT_BUFFER_SIZE];

ADC_Context_t ADC_Context = { RESULT_BUFFER_SIZE, 0, 0, 0, 0, 0, false, false, false };
AdcSummaryData g_data;

void AdcRoundSize(uint32_t dac_samples_per_period)
{
	// g_ResultBufferSize % dac_samples_per_period == 0
	ADC_Context.g_ResultBufferSize = (RESULT_BUFFER_SIZE / dac_samples_per_period) * dac_samples_per_period;
}

void AdcStop()
{
	StopTimer();
	ADC_Context.g_adc_elapsed_time = GetTime();
	ADC_Context.g_adcStatus = 2;

	ADC_StopConversion(ADC3);
	ADC_StopConversion(ADC4);
}

// IRQ Handler for ADC34
void ADC_VI_IRQHandler(void)
{
	if (DMA2->ISR & DMA_ISR_TCIF5)	// transfer complete
		ADC_Context.g_adcStatus++;
	DMA2->IFCR = DMA_IFCR_CGIF5;
}

void NVIC_Configuration34(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);

	NVIC_InitStructure.NVIC_IRQChannel = DMA2_Channel5_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

void AdcInit34(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_ADCCLKConfig(RCC_ADC34PLLCLK_Div1);

	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA2, ENABLE);
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_ADC34, ENABLE);

	NVIC_Configuration34();

	GPIO_StructInit(&GPIO_InitStructure);

	RCC_AHBPeriphClockCmd(V_ADC_RCC, ENABLE);
	GPIO_InitStructure.GPIO_Pin = V_ADC_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(V_ADC_PORT, &GPIO_InitStructure);

	RCC_AHBPeriphClockCmd(I_ADC_RCC, ENABLE);
	GPIO_InitStructure.GPIO_Pin = I_ADC_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(I_ADC_PORT, &GPIO_InitStructure);

	ADC_VoltageRegulatorCmd(ADC3, ENABLE);
	ADC_VoltageRegulatorCmd(ADC4, ENABLE);
	delay_us(20);

	ADC_SelectCalibrationMode(ADC3, ADC_CalibrationMode_Single);
	ADC_StartCalibration(ADC3);
	while (ADC_GetCalibrationStatus(ADC3) != RESET)
		;

	ADC_SelectCalibrationMode(ADC4, ADC_CalibrationMode_Single);
	ADC_StartCalibration(ADC4);
	while (ADC_GetCalibrationStatus(ADC4) != RESET)
		;

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
	if (1)
	{
		ADC_InitStructure.ADC_ContinuousConvMode = ADC_ContinuousConvMode_Disable;
		ADC_InitStructure.ADC_ExternalTrigConvEvent = ADC_ExternalTrigConvEvent_1;
		ADC_InitStructure.ADC_ExternalTrigEventEdge = ADC_ExternalTrigEventEdge_RisingEdge;
	}
	else
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

void AdcStartPre34()
{
	DMA_InitTypeDef DMA_InitStructure;

	DMA_DeInit(DMA2_Channel5);
	DMA_DeInit(DMA2_Channel2);

	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&ADC3->DR;
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)&g_resultBuffer[0];
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
	DMA_InitStructure.DMA_BufferSize = ADC_Context.g_ResultBufferSize;
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
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)&g_resultBuffer[ADC_Context.g_ResultBufferSize / 2];
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
	DMA_InitStructure.DMA_BufferSize = ADC_Context.g_ResultBufferSize;
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

	uint8_t sample_ticks = DacSampleTicks() < 72 ? ADC_SampleTime_7Cycles5 : ADC_SampleTime_19Cycles5;
	// uint8_t sample_ticks = ADC_SampleTime_601Cycles5;
	ADC_RegularChannelConfig(ADC3, ADC_Channel_1,/* PB1  */  1, sample_ticks);
	ADC_RegularChannelConfig(ADC4, ADC_Channel_1,/* PE14 */ 1, sample_ticks);

	ADC_Cmd(ADC3, ENABLE);
	ADC_Cmd(ADC4, ENABLE);
	ADC_DMACmd(ADC3, ENABLE);
	ADC_DMACmd(ADC4, ENABLE);
	ADC_Context.g_adcStatus = 1;
	ADC_Context.g_adc_cycles = 0;
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
	ADC_Context.g_adc_cur_read_pos = 0;
	ADC_Context.g_adc_read_buffer = true;
	USBAdd32(ADC_Context.g_ResultBufferSize);
	USBAdd32(ADC_Context.g_adc_elapsed_time);
	USBAdd32(ADC_Context.g_adc_cycles);
}

void AdcReadBufferComplete()
{
	ADC_Context.g_usb_sampled_data = false;
	ADC_Context.g_adc_read_buffer = false;
}

bool AdcUsbBufferComplete()
{
	return ADC_Context.g_usb_sampled_data;
}

void AdcUsbRequestData()
{
	ADC_Context.g_usb_request_data = true;
	ADC_Context.g_usb_sampled_data = false;
}

void AdcUsbReadBuffer()
{
	if (ADC_Context.g_adc_cur_read_pos >= ADC_Context.g_ResultBufferSize)
	{
		AdcReadBufferComplete();
		return;
	}

	uint32_t * buffer = g_resultBufferCopy;
	uint32_t sz = ADC_Context.g_ResultBufferSize - ADC_Context.g_adc_cur_read_pos;
	// const uint32_t max_elements = VIRTUAL_COM_PORT_DATA_SIZE/sizeof(buffer[0]);
	const uint32_t max_elements = VIRTUAL_COM_PORT_DATA_SIZE / sizeof(buffer[0]) / 2; //BUGFIX Чтото не понимаю в USB
	uint32_t to_send = sz > max_elements ? max_elements : sz;

	USBAdd((uint8_t *)(buffer + ADC_Context.g_adc_cur_read_pos), to_send * sizeof(buffer[0]));
	ADC_Context.g_adc_cur_read_pos += to_send;

	USBSend();

	if (ADC_Context.g_adc_cur_read_pos >= ADC_Context.g_ResultBufferSize)
		AdcReadBufferComplete();
}


void AdcDacStartSynchro(uint32_t period, uint16_t amplitude)
{
	if (ADC_Context.g_adcStatus == 1)
		AdcStop();

	ADC_Context.g_cur_cycle = 0;

	LcdRepaint();

	DacSetPeriod(period, amplitude);
	AdcRoundSize(DacSamplesPerPeriod());
	AdcStartPre();

	USBAdd32(DacPeriod());
	USBAdd32(SystemCoreClock);
	USBAdd32(DacSamplesPerPeriod());

	ADC_StartConversion(ADC3);
	ADC_StartConversion(ADC4);

	ADC_Context.g_adc_elapsed_time = 0;
	StartTimer();
	TIM_Cmd(TIM2, ENABLE); // Start DAC
}


void AdcResultBufferCopy(uint16_t offset, uint16_t count)
{
	uint16_t i;

	uint16_t * inV = offset + (uint16_t *)g_resultBuffer;
	uint16_t * inI = offset + (uint16_t *)&g_resultBuffer[ADC_Context.g_ResultBufferSize / 2];

	uint16_t * outV = offset + (uint16_t *)g_resultBufferCopy;
	uint16_t * outI = offset + (uint16_t *)&g_resultBufferCopy[ADC_Context.g_ResultBufferSize / 2];

	for (i = 0; i < count; i++)
		outV[i] = inV[i];

	for (i = 0; i < count; i++)
		outI[i] = inI[i];
}

//	Before start new frame
//	(must works faster as possible)
bool AdcOnNextRequest()
{
	return ADC_Context.g_usb_request_data;
}

//
//	After data copied to g_resultBufferCopy
//
void AdcOnComplete()
{
	ADC_Context.g_usb_request_data = false;

	uint16_t* inV = (uint16_t*)g_resultBufferCopy;
	uint16_t* inI = (uint16_t*)&g_resultBufferCopy[ADC_Context.g_ResultBufferSize / 2];

	g_data.count = ADC_Context.g_ResultBufferSize;

	AdcCalcData(&g_data, inV, inI, ADC_Context.g_ResultBufferSize);

	ProcessData();

	if (ProcessGetState() == STATE_NOP)
		ADC_Context.g_usb_sampled_data = true;
	else
		ADC_Context.g_usb_request_data = true;
}

void AdcQuant()
{
	uint16_t curOffset = 0;
	uint16_t counter, nextOffset;

	if (ADC_Context.g_adcStatus != 1)
		return;

	//	if(g_usb_sampled_data)
	//		return;

	if (ADC_Context.g_cur_cycle == ADC_Context.g_adc_cycles)
		return;

	ADC_Context.g_cur_cycle = ADC_Context.g_adc_cycles;

	if (DMA2_Channel5->CNDTR < ADC_Context.g_ResultBufferSize - (ADC_Context.g_ResultBufferSize / 4))
		return;	// No time to copy frame, skip it

	if (!AdcOnNextRequest())
		return;

	while (1)
	{
		counter = (uint16_t)DMA2_Channel5->CNDTR;	// Remaining data to write
		nextOffset = ADC_Context.g_ResultBufferSize - counter;
		if (ADC_Context.g_cur_cycle != ADC_Context.g_adc_cycles)
			break;

		if (curOffset < nextOffset)
		{
			AdcResultBufferCopy(curOffset, nextOffset - curOffset);
			curOffset = nextOffset;
		}
	}

	if (curOffset < ADC_Context.g_ResultBufferSize)
		AdcResultBufferCopy(curOffset, ADC_Context.g_ResultBufferSize - curOffset);

	ADC_Context.g_cur_cycle++;
	AdcClearData(&g_data);

	if (ADC_Context.g_cur_cycle != ADC_Context.g_adc_cycles)
		g_data.error = true;

	g_data.nop_number = DMA2_Channel5->CNDTR;
	AdcOnComplete();
}

void AdcSendLastComputeCh(AdcSummaryChannel * ch)
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

	ADC_Context.g_usb_sampled_data = false;
}
