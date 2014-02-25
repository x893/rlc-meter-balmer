#include "hw_config.h"

void SPI1_send(uint16_t data);

/*
	Init SPI for operate MCP6S21 Opertional amplifier
*/
void MCPInit(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;		
	// enable clock for used IO pins
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOD, ENABLE);
	/*
	PD6 - CS_I
	PD7 - CS_A
	*/
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOD, &GPIO_InitStruct);
	
	GPIO_SetBits(GPIOD, GPIO_Pin_6);
	GPIO_SetBits(GPIOD, GPIO_Pin_7);
}

/*
0 - Gain of +1 (Default)
1 - Gain of +2
2 - Gain of +4
3 - Gain of +5
4 - Gain of +8
5 - Gain of +10
6 - Gain of +16
7 - Gain of +32
*/
void MCPSetGain(bool voltage, uint8_t gain)
{
	GPIO_ResetBits(GPIOD, voltage?GPIO_Pin_7:GPIO_Pin_6);
	uint16_t command = (0b01000000<<8);
	command |= gain;

	SPI1_send(command);

	GPIO_SetBits(GPIOD, voltage?GPIO_Pin_7:GPIO_Pin_6);
}
