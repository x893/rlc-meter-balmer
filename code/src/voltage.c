// balmer@inbox.ru 2013 ESR Demo
#include "hw_config.h"
#include "voltage.h"
#include "SysTick/systick.h"
#include "SysTick/stm32f10x_systick.h"
#include "usb_desc.h"
#include "dac.h"
#include "adc.h"

void GetClockFrequency(void)
{
	RCC_ClocksTypeDef RCC_Clocks;
	//int k;
	RCC_GetClocksFreq(&RCC_Clocks);
	//k=0;
}

void StartTimer(void)
{
    SysTick_CounterCmd(SysTick_Counter_Disable);
    SysTick_SetReload(0xFFFFFF);
    SysTick_CounterCmd(SysTick_Counter_Clear);
    SysTick_CounterCmd(SysTick_Counter_Enable);
}

void StopTimer(void)
{
	SysTick_CounterCmd(SysTick_Counter_Disable);
}

uint32_t GetTime(void)//72 MHz
{
	return 0xFFFFFF-SysTick_GetCounter();
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
		DacStart();
		USBAdd32(DacPeriod());
		USBAdd32(SystemCoreClock);
		break;

	case 3://COMMAND_ADC_START
		AdcStart();
		break;
	case 4://COMMAND_ADC_READ_BUFFER
		AdcStartReadBuffer();
		break;
	case 5://COMMAND_ADC_ELAPSED_TIME
		USBAdd32(g_adc_elapsed_time);
		break;
	case 6://COMMAND_START_SYNCHRO
		AdcDacStartSynchro(*(uint32_t*)(commandBuffer+1), *(uint8_t*)(commandBuffer+5));
		break;
	}

	USBSend();
}
