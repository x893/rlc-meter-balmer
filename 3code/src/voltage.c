// balmer@inbox.ru 2013 RLC Meter
#include "hw_config.h"
#include "voltage.h"
#include "systick.h"
#include "usb_desc.h"
#include "dac.h"
#include "adc.h"
#include "mcp6s21.h"

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
		USBAdd8(commandBuffer[1]);
		break;
	case 2://COMMAND_SET_FREQUENCY
		DacSetFrequency(*(uint32_t*)(commandBuffer+1));
		DacStart();
		USBAdd32(DacPeriod());
		USBAdd32(SystemCoreClock);
		break;
	case 3://COMMAND_SET_GAIN
		MCPSetGain(commandBuffer[1]?true:false, commandBuffer[2]);
		break;
	case 4://COMMAND_ADC_READ_BUFFER
		AdcUsbStartReadBuffer();
		break;
	case 5://COMMAND_ADC_ELAPSED_TIME
		USBAdd32(g_adc_elapsed_time);
		break;
	case 6://COMMAND_START_SYNCHRO
		AdcDacStartSynchro(*(uint32_t*)(commandBuffer+1));
		break;
	case 7://COMMAND_SET_RESISTOR
		SetResistor(commandBuffer[1]);
		USBAdd8(commandBuffer[1]);
		break;
	case 8://COMMAND_LAST_COMPUTE
		AdcSendLastCompute();
		break;
	case 9://COMMAND_REQUEST_DATA
		AdcUsbRequestData();
		break;
	case 10://COMMAND_DATA_COMPLETE
		USBAdd8(AdcUsbBufferComplete()?1:0);
		break;
	case 11://COMMAND_SET_LOW_PASS
		SetLowPassFilter(commandBuffer[1]?true:false);
		break;
	}

	USBSend();
}
