#include "pcd8544.h"
#include "hw_pcd8544.h"
#include "systick.h"

#define HARDWARE_SPI

//  SPI1
//	PB3 = SCK
//	PB5 = MOSI
//	PD2 = RST
//	PD3 = CE
//	PD4 = DC
void HwLcdInit()
{
	GPIO_InitTypeDef GPIO_InitStruct;
	SPI_InitTypeDef SPI_InitStruct;
	
	// enable clock for used IO pins
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOD, ENABLE);
	
	// configure RST, CE, DC
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOD, &GPIO_InitStruct);

#ifdef HARDWARE_SPI
	{ //hardware SPI
		// enable clock for used IO pins
		RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);
		
		// configure pins used by SPI
		GPIO_InitStruct.GPIO_Pin = GPIO_Pin_3 | GPIO_Pin_5;
		GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
		GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
		GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
		GPIO_Init(GPIOB, &GPIO_InitStruct);
		
		// connect SPI1 pins to SPI alternate function
		GPIO_PinAFConfig(GPIOB, GPIO_PinSource3, GPIO_AF_5);
		GPIO_PinAFConfig(GPIOB, GPIO_PinSource5, GPIO_AF_5);

		// enable peripheral clock
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);
		
		// configure SPI1 in Mode 0 
		// CPOL = 0 --> clock is low when idle
		// CPHA = 0 --> data is sampled at the first edge
		SPI_InitStruct.SPI_Direction = SPI_Direction_1Line_Tx;
		SPI_InitStruct.SPI_Mode = SPI_Mode_Master;
		SPI_InitStruct.SPI_DataSize = SPI_DataSize_16b;
		SPI_InitStruct.SPI_CPOL = SPI_CPOL_Low;
		SPI_InitStruct.SPI_CPHA = SPI_CPHA_1Edge;
		SPI_InitStruct.SPI_NSS = SPI_NSS_Soft | SPI_NSSInternalSoft_Set;
		SPI_InitStruct.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_32;
		SPI_InitStruct.SPI_FirstBit = SPI_FirstBit_MSB;
		SPI_Init(SPI1, &SPI_InitStruct); 
		
		SPI_Cmd(SPI1, ENABLE);

	}
#else
	{ //software SPI
		RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);
		
		// configure pins used by SPI1
		GPIO_InitStruct.GPIO_Pin = GPIO_Pin_3 | GPIO_Pin_5;
		GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
		GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
		GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
		GPIO_Init(GPIOB, &GPIO_InitStruct);
		GPIO_WriteBit(GPIOB, GPIO_Pin_3, 1);
	}
#endif
}

void DelaySome()
{
	delay_ms(50);
}

void HwLcdPinCE(uint8_t on)
{
	GPIO_WriteBit(GPIOD, GPIO_Pin_3, on);
}

void HwLcdPinDC(uint8_t on)
{
	GPIO_WriteBit(GPIOD, GPIO_Pin_4, on);
}

void HwLcdPinRst(uint8_t on)
{
	GPIO_WriteBit(GPIOD, GPIO_Pin_2, on);
}

void SPI1_send(uint16_t data)
{
#ifdef HARDWARE_SPI
	SPI1->DR = data;
	while( !(SPI1->SR & SPI_I2S_FLAG_TXE) ); // wait until transmit complete
	while( SPI1->SR & SPI_I2S_FLAG_BSY ); // wait until SPI is not busy anymore
#else
	unsigned char i;
	for(i=0; i<16; i++) {
		for(volatile int j=0; j<4; j++);
		GPIO_WriteBit(GPIOB, GPIO_Pin_5, (data & 0x8000)? 1:0);
		data = data<<1;
		for(volatile int j=0; j<32; j++);
		GPIO_WriteBit(GPIOB, GPIO_Pin_3, 0);
		for(volatile int j=0; j<32; j++);
		GPIO_WriteBit(GPIOB, GPIO_Pin_3, 1);
	}
#endif
}

void HwLcdSend(uint16_t data)
{
	SPI1_send(data);
}
