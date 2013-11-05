// balmer@inbox.ru 2013 ESR Demo
#include "hw_config.h"
#include "voltage.h"
#include "SysTick/systick.h"
#include "SysTick/stm32f10x_systick.h"
#include "usb_desc.h"
#include "sound.h"
#include "dac.h"

uint16_t adc_buffer[ADC_BUFFER_SIZE];
uint32_t adc_buffer_pos = 0;
uint32_t adc_buffer_pos_get_result;
uint8_t adc_get_result_command = 0; //USB command get adc result proceded
volatile uint8_t g_start_conversion = 0;
uint8_t g_conversion_voltage = 0;

void GetClockFrequency(void)
{
	RCC_ClocksTypeDef RCC_Clocks;
	//int k;
	RCC_GetClocksFreq(&RCC_Clocks);
	//k=0;
}

void StartTimer(void)
{
    SysTick_SetReload(0xFFFFFF);
    SysTick_CounterCmd(SysTick_Counter_Clear);
    SysTick_CounterCmd(SysTick_Counter_Enable);
}

void StopTimer(void)
{
    SysTick_CounterCmd(SysTick_Counter_Disable);
	SysTick_CounterCmd(SysTick_Counter_Clear);
}

uint32_t GetTime(void)//HSE/8 = 48/8 = 6 MHz
{
	return 0xFFFFFF-SysTick_GetCounter();
}

void ADC_Configuration(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	ADC_InitTypeDef ADC_InitStructure;

	RCC_ADCCLKConfig(RCC_PCLK2_Div4);//48/4 = 12 MHz
	//RCC_ADCCLKConfig(RCC_PCLK2_Div8);//48/8 = 6 MHz

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1 | RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE);

	/* Configure PA.0 (ADC Channel10) as analog input -------------------------*/
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_Init(GPIOA, &GPIO_InitStructure);   

	/* ADC1 configuration ------------------------------------------------------*/
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
	ADC_InitStructure.ADC_ScanConvMode = ENABLE;
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
	ADC_InitStructure.ADC_NbrOfChannel = 1;
	ADC_Init(ADC1, &ADC_InitStructure);

	/* ADC1 regular channel10 configuration */ 
	ADC_RegularChannelConfig(ADC1, ADC_Channel_0, 1, ADC_SampleTime_13Cycles5); // 13.5clock+wait  2.19 us time on 12 mhz
	//ADC_RegularChannelConfig(ADC1, ADC_Channel_0, 1, ADC_SampleTime_55Cycles5);
	ADC_Cmd(ADC1, ENABLE);

	/* Enable ADC1 reset calibaration register */
	ADC_ResetCalibration(ADC1);
	/* Check the end of ADC1 reset calibration register */
	while(ADC_GetResetCalibrationStatus(ADC1)); /* Start ADC1 calibaration */
	ADC_StartCalibration(ADC1); /* Check the end of ADC1 calibration */
	while(ADC_GetCalibrationStatus(ADC1));

	ADC_SoftwareStartConvCmd(ADC1, ENABLE);

	GetClockFrequency();
}

uint16_t getADCValue()
{
	uint16_t AD_value = 0xFFFF;
	if(ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC)==SET)
	{
		AD_value=ADC_GetConversionValue(ADC1);
	}
	return AD_value;
}


void voltageInit(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	// configure voltage pins
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5|GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	voltageSet(VoltageOff);
	ADC_Configuration();
}



void voltageSet(enum VOLTAGE_ENUM type)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	//Off All
	GPIO_SetBits(GPIOA, GPIO_Pin_5);
	GPIO_ResetBits(GPIOA, GPIO_Pin_6);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3|GPIO_Pin_4;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	switch(type)
	{
	case VoltageOff:
		break;
	case VoltageZero:
		GPIO_SetBits(GPIOA, GPIO_Pin_6);
		break;
	case VoltageUp1:
		GPIO_ResetBits(GPIOA, GPIO_Pin_5);
		break;
	case VoltageUp2:
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
		GPIO_Init(GPIOA, &GPIO_InitStructure);
		GPIO_SetBits(GPIOA, GPIO_Pin_4);
		break;
	case VoltageUp3:
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
		GPIO_Init(GPIOA, &GPIO_InitStructure);
		GPIO_SetBits(GPIOA, GPIO_Pin_3);
		break;
	default:
		//assert_param(0);
		;
	}
}

void USBCommandReceive(uint8_t* commandBuffer, uint16_t commandSize)
{
	if(commandSize==0)
		return;
	USBAdd8(commandBuffer[0]);
	switch(commandBuffer[0])
	{
	case 1://COMMAND_SET_LED
		USB_SetLeds(commandBuffer[1]);
		USBAdd8(commandBuffer[1]);
		break;
	case 2://COMMAND_SET_FREQUENCY
		DacSetFrequency(*(uint32_t*)(commandBuffer+1));
		USBAdd32(DacPeriod());
		USBAdd32(SystemCoreClock);
		break;
	/*
	case 1://COMMAND_SET_VOLTAGE
		USBAdd8(commandBuffer[0]);
		voltageSet(commandBuffer[1]);
		USBAdd8(commandBuffer[1]);
		break;
	case 2://COMMAND_GET_ADC
		USBAdd8(commandBuffer[0]);
		USBAdd16(getADCValue());
		break;
	case 3://COMMAND_CONVERSION
		USBAdd8(commandBuffer[0]);
		g_conversion_voltage = commandBuffer[1];
		g_start_conversion = 1;
		return;
	case 4://COMMAND_GET_RESULT
		adc_buffer_pos_get_result = 0;
		adc_get_result_command = 1;
		SendConversionResult();
		break;
	case 5://COMMAND_SOUND
		//destructive command, voltage conversion not vorked after this
		sound_command = true;
		voltageSet(VoltageUp3);
		SoundInit(commandBuffer[1]);
		break;
	*/
	}

	USBSend();
}

void ReceiveADCData(void)
{
	uint32_t startT, stopT, dT;
	uint16_t adcValue;
	ADC_SoftwareStartConvCmd(ADC1, DISABLE);
	adc_buffer_pos = 0;
	voltageSet(VoltageZero);
	delay_ms(20);
	voltageSet(VoltageOff);
	ADC_ClearITPendingBit(ADC1, ADC_IT_EOC);
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);
	//Skip first sample to synchronize
	while(ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC)!=SET);
	adcValue = ADC_GetConversionValue(ADC1);

	StartTimer();
	startT = GetTime();
	voltageSet(g_conversion_voltage);

	while(adc_buffer_pos<ADC_BUFFER_SIZE)
	{
		while(ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC)!=SET);
		adcValue = ADC_GetConversionValue(ADC1);
		adc_buffer[adc_buffer_pos++] = adcValue;
		if(adcValue>=4095)
			break;
	}

	stopT = GetTime();

	ADC_SoftwareStartConvCmd(ADC1, DISABLE);
	voltageSet(VoltageZero);

	dT = stopT - startT;
	USBAdd32(dT);//
	USBAdd32(adc_buffer_pos);//count samples
	USBSend();

	StopTimer();
}

void SendConversionResult()
{
	uint16_t maxSamples = VIRTUAL_COM_PORT_DATA_SIZE/2;
	uint16_t maxi = adc_buffer_pos_get_result+maxSamples;
	if(maxi>adc_buffer_pos)
	{
		maxi = adc_buffer_pos;
		adc_get_result_command = 0;
	}

	for(; adc_buffer_pos_get_result<maxi; adc_buffer_pos_get_result++)
		USBAdd16(adc_buffer[adc_buffer_pos_get_result]);
}
