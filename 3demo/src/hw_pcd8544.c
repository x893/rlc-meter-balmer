#include "pcd8544.h"
#include "hw_pcd8544.h"

#include "stm32f30x.h"
void Delay(__IO uint32_t nTime);

#define HARDWARE_SPI

/*
	PB13 = SCK
	PB15 = MOSI
	PB14 = RST
	PD12 = CE
	PD10 = DC
*/
void HwLcdInit()
{
	GPIO_InitTypeDef GPIO_InitStruct;
	SPI_InitTypeDef SPI_InitStruct;
	
	// enable clock for used IO pins
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);
	
	// configure RST, CE, DC
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_12 | GPIO_Pin_14;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOB, &GPIO_InitStruct);

#ifdef HARDWARE_SPI
	{ //hardware SPI
		// enable clock for used IO pins
		RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);
		
		// configure pins used by SPI
		GPIO_InitStruct.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_15;
		GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
		GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
		GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
		GPIO_Init(GPIOB, &GPIO_InitStruct);
		
		// connect SPI2 pins to SPI alternate function
		GPIO_PinAFConfig(GPIOB, GPIO_PinSource13, GPIO_AF_5);
		GPIO_PinAFConfig(GPIOB, GPIO_PinSource15, GPIO_AF_5);
				
		// enable peripheral clock
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);

		SPI_I2S_DeInit(SPI2);
		
		/* configure SPI1 in Mode 0 
		 * CPOL = 0 --> clock is low when idle
		 * CPHA = 0 --> data is sampled at the first edge
		 */
		SPI_InitStruct.SPI_Direction = SPI_Direction_1Line_Tx;
		SPI_InitStruct.SPI_Mode = SPI_Mode_Master;
		SPI_InitStruct.SPI_DataSize = SPI_DataSize_16b;
		SPI_InitStruct.SPI_CPOL = SPI_CPOL_Low;
		SPI_InitStruct.SPI_CPHA = SPI_CPHA_1Edge;
		SPI_InitStruct.SPI_NSS = SPI_NSS_Soft | SPI_NSSInternalSoft_Set;
		SPI_InitStruct.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_32;
		SPI_InitStruct.SPI_FirstBit = SPI_FirstBit_MSB;
		SPI_Init(SPI2, &SPI_InitStruct); 
		
		SPI_Cmd(SPI2, ENABLE);
	}
#else
	{ //software SPI
		RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);
		
		// configure pins used by SPI1
		GPIO_InitStruct.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_15;
		GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
		GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
		GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
		GPIO_Init(GPIOB, &GPIO_InitStruct);
		GPIO_WriteBit(GPIOB, GPIO_Pin_13, 1);
	}
#endif
}

void DelaySome()
{
	Delay(5);
}

void HwLcdPinCE(uint8_t on)
{
	GPIO_WriteBit(GPIOB, GPIO_Pin_12, on);
}

void HwLcdPinDC(uint8_t on)
{
	GPIO_WriteBit(GPIOB, GPIO_Pin_10, on);
}

void HwLcdPinRst(uint8_t on)
{
	GPIO_WriteBit(GPIOB, GPIO_Pin_14, on);
}

void HwLcdSend(uint16_t data)
{
#ifdef HARDWARE_SPI
	SPI2->DR = data;
	while( !(SPI2->SR & SPI_I2S_FLAG_TXE) ); // wait until transmit complete
	while( SPI2->SR & SPI_I2S_FLAG_BSY ); // wait until SPI is not busy anymore
#else
	unsigned char i;
	for(i=0; i<16; i++) {
		GPIO_WriteBit(GPIOB, GPIO_Pin_15, (data & 0x80)? 1:0);
		data = data<<1;
		for(volatile int j=0; j<32; j++);
		GPIO_WriteBit(GPIOB, GPIO_Pin_13, 0);
		for(volatile int j=0; j<32; j++);
		GPIO_WriteBit(GPIOB, GPIO_Pin_13, 1);
	}
#endif
}
