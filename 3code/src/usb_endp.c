
#include "usb_lib.h"
#include "usb_istr.h"
#include "voltage.h"
#include "adc.h"

#include <string.h>

static uint8_t USB_Rx_Buffer[VIRTUAL_COM_PORT_DATA_SIZE];
static uint8_t Rx_Buffer [VIRTUAL_COM_PORT_DATA_SIZE]; 
static uint32_t Rx_length  = 0;

void USBAdd(uint8_t* data, uint32_t size)
{
  uint32_t i;
  if(Rx_length+size>VIRTUAL_COM_PORT_DATA_SIZE)
    size = VIRTUAL_COM_PORT_DATA_SIZE - Rx_length;
  for (i=0; i<size; i++)
    Rx_Buffer[Rx_length++] = data[i];
}

void USBAddStr(char* data)
{
  USBAdd((uint8_t*)data, strlen(data));
}

void USBAdd8(uint8_t data)
{
  USBAdd((uint8_t*)&data, sizeof(data));
}

void USBAdd16(uint16_t data)
{
  USBAdd((uint8_t*)&data, sizeof(data));
}

void USBAdd32(uint32_t data)
{
  USBAdd((uint8_t*)&data, sizeof(data));
}

void USBSend(void)
{
  USB_SIL_Write(EP1_IN, Rx_Buffer, Rx_length);
  SetEPTxValid(ENDP1); 
  Rx_length = 0;
}


void EP1_IN_Callback(void)
{
  if(g_adc_read_buffer)
    AdcUsbReadBuffer();
  else
  	USBSend();
}

void EP1_OUT_Callback(void)
{
	uint16_t USB_Rx_Cnt;
	USB_Rx_Cnt = USB_SIL_Read(EP1_OUT, USB_Rx_Buffer);
	USBCommandReceive(USB_Rx_Buffer, USB_Rx_Cnt);
/*
	switch(USB_Rx_Buffer[0])
	{
	case 0: 
		USBAddStr("0 is zero");
		break;
	case 1: 
		USBAddStr("1 is one");
		break;
	default:
		USBAddStr("default");
	}

	USBSend();
*/
	SetEPRxValid(ENDP1);
}
