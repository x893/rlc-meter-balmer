// balmer@inbox.ru 2013 RLC Meter
#include "hw_config.h"
#include "voltage.h"
#include "systick.h"
#include "usb_desc.h"
#include "dac.h"
#include "adc.h"
#include "mcp6s21.h"
#include "calc_rc.h"
#include "corrector.h"
#include "lcd_interface.h"


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
		{
			uint32_t f = *(uint32_t*)(commandBuffer+1);
			DacSetFrequency(f);
			DacStart();
			USBAdd32(DacPeriod());
			USBAdd32(SystemCoreClock);
		}
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
		AdcDacStartSynchro(*(uint32_t*)(commandBuffer+1), *(uint16_t*)(commandBuffer+5));
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
	case 12://COMMAND_START_GAIN_AUTO
		ProcessStartComputeX(commandBuffer[1]/*count*/, commandBuffer[2]/*predefinedResistorIdx*/);
		break;
	case 13://COMMAND_RVI_INDEXES
		SendRVI();
		break;
	case 14://COMMAND_SET_CORRECTOR2XR
		USBAdd8(commandBuffer[1]);
		SetCorrector2xR(commandBuffer[1], (float*)(commandBuffer+4));
		break;
	case 15://COMMAND_SET_CORRECTOR2X
		USBAdd8(commandBuffer[1]);
		USBAdd8(commandBuffer[2]);
		SetCorrector2x(commandBuffer[1], commandBuffer[2], (float*)(commandBuffer+4));
		break;
	case 16://COMMAND_SET_CORRECTOR_OPENR
		SetCorrectorOpenR(commandBuffer[1], (float*)(commandBuffer+4));
		break;
	case 17://COMMAND_SET_CORRECTOR_OPEN
		USBAdd8(commandBuffer[1]);
		SetCorrectorOpen(commandBuffer[1], (float*)(commandBuffer+4));
		break;
	case 18://COMMAND_SET_CORRECTOR_SHORTR
		SetCorrectorShortR((float*)(commandBuffer+4));
		break;
	case 19://COMMAND_SET_CORRECTOR_SHORT
		USBAdd8(commandBuffer[1]);
		SetCorrectorShort(commandBuffer[1], (float*)(commandBuffer+4));
		break;
	case 20://COMMAND_SET_CORRECTOR_PERIOD
		SetCorrectorPeriod(*(uint32_t*)(commandBuffer+4));
		break;
	case 21://COMMAND_CORRECTOR_FLASH_CLEAR
		USBAdd8(CorrectorFlashClear());
		break;
	case 22://COMMAND_FLASH_CURRENT_DATA
		USBAdd8(CorrectorFlashCurrentData());
		break;
	case 23://COMMAND_SET_SERIAL
		isSerial = commandBuffer[1]?true:false;
		LcdRepaint();
		break;
	case 24://COMMAND_SET_CONTINUOUS_MODE
		bContinuousMode = commandBuffer[1]?true:false;
		break;
	}

	USBSend();
}
