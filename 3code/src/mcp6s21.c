// balmer@inbox.ru RLC Meter 303
// 2013-2014

#include "hw_config.h"

void HwLcdSend(uint16_t data);

/*
	Init SPI for operate MCP6S21 Opertional amplifier
 */
void MCPInit(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;

	// enable clock for used IO pins
	RCC_AHBPeriphClockCmd(SPI_CS_RCC, ENABLE);

	GPIO_SetBits(SPI_CS_PORT, SPI_CS_I_PIN);
	GPIO_SetBits(SPI_CS_PORT, SPI_CS_V_PIN);

	/*
	PD6 - CS_I
	PD7 - CS_A
	*/
	GPIO_InitStruct.GPIO_Pin = SPI_CS_I_PIN | SPI_CS_V_PIN;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(SPI_CS_PORT, &GPIO_InitStruct);

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
	uint16_t command = (0x40 << 8) | gain;

	GPIO_ResetBits(SPI_CS_PORT, voltage ? SPI_CS_V_PIN : SPI_CS_I_PIN);
	HwLcdSend(command);
	GPIO_SetBits(SPI_CS_PORT, voltage ? SPI_CS_V_PIN : SPI_CS_I_PIN);
}
