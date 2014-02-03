#include "hw_config.h"
#include "pressure.h"

/*
	Init SPI for operate MCP6S21 Opertional amplifier
*/
void PressureInit(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	SPI_InitTypeDef SPI_InitStruct;
	
	// enable clock for used IO pins
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);
	
	/* configure pins used by SPI1
	 * PB3 = SCK
	 * PB5 = MOSI
	 * PB4 = MISO
	 */
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOB, &GPIO_InitStruct);
	
	// connect SPI1 pins to SPI alternate function
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource3, GPIO_AF_5);
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource4, GPIO_AF_5);
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource5, GPIO_AF_5);
	
	// enable clock for used IO pins
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOD, ENABLE);
	
	/*
	PB6 - CS
	*/
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_6;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOB, &GPIO_InitStruct);
	
	GPIO_SetBits(GPIOB, GPIO_Pin_6);
	
	// enable peripheral clock
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);
	
	/* configure SPI1 in Mode 0 
	 * CPOL = 0 --> clock is low when idle
	 * CPHA = 0 --> data is sampled at the first edge
	 */
	SPI_InitStruct.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	SPI_InitStruct.SPI_Mode = SPI_Mode_Master;
	SPI_InitStruct.SPI_DataSize = SPI_DataSize_16b;
	SPI_InitStruct.SPI_CPOL = SPI_CPOL_Low;
	SPI_InitStruct.SPI_CPHA = SPI_CPHA_1Edge;
	SPI_InitStruct.SPI_NSS = SPI_NSS_Soft | SPI_NSSInternalSoft_Set;
	SPI_InitStruct.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_256;
	SPI_InitStruct.SPI_FirstBit = SPI_FirstBit_MSB;
	SPI_Init(SPI1, &SPI_InitStruct); 
	
	SPI_Cmd(SPI1, ENABLE);
}

static uint16_t SPI1_send(uint16_t data)
{
	SPI1->DR = data; // write data to be transmitted to the SPI data register
	while( !(SPI1->SR & SPI_I2S_FLAG_TXE) ); // wait until transmit complete
	while( !(SPI1->SR & SPI_I2S_FLAG_RXNE) ); // wait until receive complete
	while( SPI1->SR & SPI_I2S_FLAG_BSY ); // wait until SPI is not busy anymore
	return SPI1->DR; // return received data from SPI data register
}

uint16_t PressureSend(uint16_t data)
{
	GPIO_ResetBits(GPIOB, GPIO_Pin_6);
	uint16_t out = SPI1_send(data);

	GPIO_SetBits(GPIOB, GPIO_Pin_6);
	return out;
}

uint16_t PressureRead(uint8_t reg)
{
	uint16_t data = reg;

	data = (data<<8)|0x8000;
	return PressureSend(data)&0xFF;
}

void PressureWrite(uint8_t reg, uint8_t value)
{
	uint16_t data = reg;
	data = (data<<8)|value;
	PressureSend(data);
}

int16_t PressureReadTemp()
{
    uint16_t temp_lo = PressureRead(0x2B);
    uint16_t temp_hi = PressureRead(0x2C);
    return ((int16_t)((temp_hi<<8)|temp_lo))/48+425;
}

uint16_t PressureReadPressure()
{
    uint32_t p_xl = PressureRead(0x28);
    uint32_t p_l = PressureRead(0x29);
    uint32_t p_h = PressureRead(0x2A);
    uint32_t p = (p_h<<16)|(p_l<<8)|(p_xl);
    return p/4096;
}
