#include <stdint.h>
#include "hw_ili9341.h"

#include "stm32f30x.h"
void Delay(__IO uint32_t nTime);

#define TFT_PORT	GPIOA
#define TFT_SPI		SPI1

#define PIN_LED		GPIO_Pin_1
#define PIN_RST		GPIO_Pin_2
#define PIN_DC		GPIO_Pin_3
#define PIN_CS		GPIO_Pin_0
#define PIN_SCK		GPIO_Pin_5
#define PIN_MISO	GPIO_Pin_6
#define PIN_MOSI	GPIO_Pin_7

#define PIN_SOURCE_SCK	GPIO_PinSource5
#define PIN_SOURCE_MISO	GPIO_PinSource6
#define PIN_SOURCE_MOSI	GPIO_PinSource7

static SPI_InitTypeDef spi;


void HwLcdInit()
{
	GPIO_InitTypeDef gpio;

	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);

	gpio.GPIO_Pin = PIN_LED | PIN_DC | PIN_RST | PIN_CS;
	gpio.GPIO_Mode = GPIO_Mode_OUT;
	gpio.GPIO_OType = GPIO_OType_PP;
	gpio.GPIO_Speed = GPIO_Speed_50MHz;
	gpio.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(TFT_PORT, &gpio);

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);

	gpio.GPIO_Pin = PIN_SCK | PIN_MOSI;
	gpio.GPIO_Mode = GPIO_Mode_AF;
	gpio.GPIO_OType = GPIO_OType_PP;
	gpio.GPIO_Speed = GPIO_Speed_50MHz;
	gpio.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(TFT_PORT, &gpio);

    gpio.GPIO_Pin = PIN_MISO;
	gpio.GPIO_Mode = GPIO_PuPd_UP;
    GPIO_Init(TFT_PORT, &gpio);

	// connect SPI1 pins to SPI alternate function
	GPIO_PinAFConfig(TFT_PORT, PIN_SOURCE_SCK, GPIO_AF_5);
	GPIO_PinAFConfig(TFT_PORT, PIN_SOURCE_MISO, GPIO_AF_5);
	GPIO_PinAFConfig(TFT_PORT, PIN_SOURCE_MOSI, GPIO_AF_5);

    //SPI_StructInit(&spi);
	spi.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	spi.SPI_Mode = SPI_Mode_Master;
	spi.SPI_DataSize = SPI_DataSize_8b;
	spi.SPI_CPOL = SPI_CPOL_Low;
	spi.SPI_CPHA = SPI_CPHA_1Edge;
	spi.SPI_NSS = SPI_NSS_Soft;
	spi.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_128;
	spi.SPI_FirstBit = SPI_FirstBit_MSB;
	SPI_Init(TFT_SPI, &spi);
	SPI_Cmd(TFT_SPI, ENABLE);
	SPI_NSSInternalSoftwareConfig(SPI1, SPI_NSSInternalSoft_Set);
}

void HwLcdChangeSpiPrescaler(uint16_t spiPrescaler)
{
	SPI_Cmd(TFT_SPI, DISABLE);
	spi.SPI_BaudRatePrescaler = spiPrescaler;
	SPI_Init(TFT_SPI, &spi);
	SPI_Cmd(TFT_SPI, ENABLE);
	SPI_NSSInternalSoftwareConfig(SPI1, SPI_NSSInternalSoft_Set);
}

void DelaySome()
{
	Delay(5);
}

void HwLcdPinCE(uint8_t on)
{
	GPIO_WriteBit(TFT_PORT, PIN_CS, on);
}

void HwLcdPinDC(uint8_t on)
{
	GPIO_WriteBit(TFT_PORT, PIN_DC, on);
}

void HwLcdPinRst(uint8_t on)
{
	GPIO_WriteBit(TFT_PORT, PIN_RST, on);
}

void HwLcdPinLed(uint8_t on)
{
	GPIO_WriteBit(TFT_PORT, PIN_LED, on);
}

uint8_t HwLcdSend(uint8_t data)
{
	SPI_SendData8(TFT_SPI, data);
	while( !(SPI1->SR & SPI_I2S_FLAG_TXE) ); // wait until transmit complete
	while( SPI1->SR & SPI_I2S_FLAG_BSY ); // wait until SPI is not busy anymore
	return 0;//temp code
}
