// balmer@inbox.ru RLC Meter 303
// 2013-2014

#include <string.h>
#include "usb_lib.h"
#include "usb_istr.h"
#include "usb_commands.h"
#include "adc.h"

uint8_t USB_Rx_Buffer[VIRTUAL_COM_PORT_DATA_SIZE];
uint8_t Rx_Buffer[VIRTUAL_COM_PORT_DATA_SIZE];
uint16_t Rx_length = 0;

void USBAdd(uint8_t * data, uint16_t size)
{
	uint16_t i;
	if (Rx_length + size > VIRTUAL_COM_PORT_DATA_SIZE)
		size = VIRTUAL_COM_PORT_DATA_SIZE - Rx_length;
	for (i = 0; i < size; i++)
		Rx_Buffer[Rx_length++] = data[i];
}

void USBAddStr(char * data)
{
	USBAdd((uint8_t *)data, strlen(data));
}

void USBAdd8(uint8_t data)
{
	USBAdd((uint8_t *)&data, sizeof(data));
}

void USBAdd16(uint16_t data)
{
	USBAdd((uint8_t *)&data, sizeof(data));
}

void USBAdd32(uint32_t data)
{
	USBAdd((uint8_t *)&data, sizeof(data));
}

void USBAddFloat(float data)
{
	USBAdd((uint8_t *)&data, sizeof(data));
}

void USBSend(void)
{
	USB_SIL_Write(EP1_IN, Rx_Buffer, Rx_length);
	SetEPTxValid(ENDP1);
	Rx_length = 0;
}

void EP1_IN_Callback(void)
{
	if (ADC_Context.g_adc_read_buffer)
		AdcUsbReadBuffer();
}

void EP1_OUT_Callback(void)
{
	USBCommandReceive(USB_Rx_Buffer, USB_SIL_Read(EP1_OUT, USB_Rx_Buffer));
	SetEPRxValid(ENDP1);
}
