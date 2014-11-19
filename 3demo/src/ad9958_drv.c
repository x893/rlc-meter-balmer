/**
  ******************************************************************************
  * @file    DRV/SARK110/ad9958_drv.c
  * @author  Melchor Varela - EA4FRB
  * @version V0.7.x
  * @date    12-December-2012
  * @brief   DDS AD9958 Driver
  ******************************************************************************
  * @copy
  *
  *  This file is a part of the "SARK110 Antenna Vector Impedance Analyzer" firmware
  *
  *  "SARK110 Antenna Vector Impedance Analyzer firmware" is free software: you can redistribute it
  *  and/or modify it under the terms of the GNU General Public License as
  *  published by the Free Software Foundation, either version 3 of the License,
  *  or (at your option) any later version.
  *
  *  "SARK110 Antenna Vector Impedance Analyzer firmware" is distributed in the hope that it will be
  *  useful,  but WITHOUT ANY WARRANTY; without even the implied warranty of
  *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  *  GNU General Public License for more details.
  *
  *  You should have received a copy of the GNU General Public License
  *  along with "SARK110 Antenna Vector Impedance Analyzer" firmware.  If not,
  *  see <http://www.gnu.org/licenses/>.
  *
  * <h2><center>&copy; COPYRIGHT 2011-2013 Melchor Varela - EA4FRB </center></h2>
  *  Melchor Varela, Madrid, Spain.
  *  melchor.varela@gmail.com
  */

/** @defgroup DRV
  * @{
  */

/** @defgroup AD9958
  * @{
  */


/* Includes ------------------------------------------------------------------*/
#include "hw_config.h"
#include "ad9958_drv.h"
#include "calibrate_det.h"

#include "stm32f30x.h"
#include "stm32f30x_gpio.h"
#include "stm32f30x_spi.h"


/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/

/** @defgroup AD9958_Private_Variables
  * @{
  */
#define PLL_MUL					20.0				/* PLL multiplier */
#define REFCLK					25000000.0			/* Reference clock: 25 MHz */
#define MASTER_CLOCK			(REFCLK*PLL_MUL)	/* Master clock: 500 MHz */

#define MAX_OUTPUT_FREQUENCY	MAX_FREQUENCY		/* 230 MHz */

/* Private macro -------------------------------------------------------------*/
/** @defgroup AD9958_Private_Macro
  * @{
  */
#ifdef DDS_POWER_DOWN_ENABLE
#define DDS_PWR_DOWN_HIGH	(GPIOE->BSRR = GPIO_Pin_3)
#define DDS_PWR_DOWN_LOW	(GPIOE->BRR = GPIO_Pin_3)
#endif

#define DDS_RESET_HIGH		(GPIOB->BSRR = GPIO_Pin_1)
#define DDS_RESET_LOW		(GPIOB->BRR = GPIO_Pin_1)

#define DDS_IO_UPD_HIGH		(GPIOB->BSRR = GPIO_Pin_0)
#define DDS_IO_UPD_LOW		(GPIOB->BRR = GPIO_Pin_0)

#define DDS_CS_HIGH			(GPIOA->BSRR = GPIO_Pin_4)
#define DDS_CS_LOW			(GPIOA->BRR = GPIO_Pin_4)


    // CSR: 3-wire mode, MSB first, channels enabled
//static uint8_t wire = 2;
    // CSR: 2-wire mode, MSB first, channels enabled
static uint8_t wire = 0;

/**
  * @}
  */

void Delay(uint32_t nTime);

/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
static void RCC_Configuration(void);
static void GPIO_Configuration(void);
static void dds_send(uint8_t u8Data);
static uint8_t dds_rcv();
static void WR_CSR(uint8_t u8Data);
static uint8_t RD_CSR(void);
static void WR_FR1(uint8_t u8Data1,uint8_t u8Data2,uint8_t u8Data3);
static void WR_FR2(uint8_t u8Data1,uint8_t u8Data2);
static void WR_CFR(uint8_t u8Data1,uint8_t u8Data2,uint8_t u8Data3);
static void WR_CFTW0(uint32_t u32ftWord);
static void WR_CPOW0(uint32_t u32CpWord);
static void WR_ACR(uint8_t u8Data1,uint8_t u8Data2,uint8_t u8Data3);
static void IOUpdatePulse(void);

#define HI8(x) (uint8_t)(x>>8)
#define LO8(x) (uint8_t)(x)


/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Initializes the AD9958 device.
  *
  * @retval None
  */
void AD9958_Init(void)
{
SPI_InitTypeDef   SPI_InitStructure;

  	/* System clocks configuration ---------------------------------------------*/
	RCC_Configuration();

  	/* GPIO configuration ------------------------------------------------------*/
  	GPIO_Configuration();
/*
  	// 1st phase: SPI1 Master 
  	// SPI1 Config -------------------------------------------------------------
  	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
  	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
  	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
  	SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
  	SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
  	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
  	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_256;//SPI_BaudRatePrescaler_4;
  	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
  	SPI_InitStructure.SPI_CRCPolynomial = 7;
  	SPI_Init(SPI1, &SPI_InitStructure);

  	// Enable SPI1
  	SPI_Cmd(SPI1, ENABLE);
*/
  	/* DDS configuration -------------------------------------------------------
  	/* Resets DDS device */
#ifdef DDS_POWER_DOWN_ENABLE    
	DDS_PWR_DOWN_LOW;
#endif
	DDS_IO_UPD_HIGH;
	DDS_CS_HIGH;

	DDS_RESET_LOW;
	DDS_RESET_HIGH;
	Delay(2);
	DDS_RESET_LOW;
	Delay(2);


    WR_CSR(0xc0|wire);

	/* FR1: PLL=20  */
 	//WR_FR1(0xd0,0x00,0x00);

    //PLL=20, Charge Pump=max
    //WR_FR1(0xd3,0x00,0x00);

    //PLL=4, Charge Pump=max
    WR_FR1(0x10,0x00,0x00);

 	/* Reset phase */
 	WR_CPOW0(0x00);

	/* Gain for main channel  */
 	WR_CSR(0x40|wire);
	WR_CFR(0x00,0x03,0x00);
	WR_ACR(0x00, 0x11, 0x80);
 	WR_CFTW0(0);

	/* Gain for LO  */
	WR_CSR(0x80|wire);
	WR_CFR(0x00,0x03,0x00);
	WR_ACR(0x00, 0x11, 0x00);
 	WR_CFTW0(0);

	/* Enable both channel cmds */
	WR_CSR(0xc0|wire);

	/* Transfer registers */
	IOUpdatePulse();
}

/**
  * @brief  Sets the DDS frequency
  *
  * @param  iChannel 	DDS Channel:
  *						@li DDS_MAIN
  *						@li DDS_LO
  * @param  dfFreq 		Frequency in Hz
  * @retval iError		Error code:
  *						@li 1: OK
  *						@li 0: Wrong argument
  */
int AD9958_Set_Frequency(int iChannel,double dfFreq)
{
 	uint32_t u32Temp;
	uint8_t u8High;
	uint8_t u8Low;
	uint16_t u16AmpWord;

    /* Argument checking */
    if((iChannel!=DDS_MAIN) && (iChannel!=DDS_LO))
    	return 0;

    /* Adjust amplitude level */
	u16AmpWord=GetAmplitudeWord(dfFreq);
	if(iChannel==DDS_MAIN)
	{
		/* Enable channel 0 cmds and disable channel 1 cmds */
		WR_CSR(0x40|wire);
	}
	else
	{
	  	/* Enable channel 1 cmds and disable channel 0 cmds */
		WR_CSR(0x80|wire);
		u16AmpWord-=0x80;
	}
	//u8High=HI8(u16AmpWord);
	//u8Low=LO8(u16AmpWord);
	//WR_ACR(0x00, u8High, u8Low);

	/* Write frequency word */
 	u32Temp=(uint32_t)dfFreq*(0xFFFFFFFF/(float)MASTER_CLOCK+1.0/MASTER_CLOCK);
 	WR_CFTW0(u32Temp);

	/* Enable both channel cmds */
	WR_CSR(0xc0|wire);

  	/* All channels clear phase for synchronization */
	WR_FR2(0x10,0x00);
	WR_FR2(0x00,0x00);

	/* Transfer registers */
	IOUpdatePulse();

	return 1;
}

/**
  * @brief  Test the communication with the DDS
  *
  * @retval bError:
  *					@li true correct
  *					@li false failure
  */
bool AD9958_Test(void)
{
	/* Perform several write and verify operations */
	WR_CSR(0x40|wire);
	if (RD_CSR()!=(0x40|wire))
		return false;
	WR_CSR(0x80|wire);
	if (RD_CSR()!=(0x80|wire))
		return false;
	WR_CSR(0xc0|wire);
	if (RD_CSR()!=(0xc0|wire))
		return false;

	return true;
}

/**
  * @brief  Sets the amplitude level for the selected channel
  *
  * @param  iChannel 	DDS Channel:
  *						@li DDS_MAIN
  *						@li DDS_LO
  * @param  u16Level 	Amplitude word
  * @retval iError		Error code:
  *						@li 1: OK
  *						@li 0: Wrong argument
  */
int AD9958_Set_Level(int iChannel,uint16_t u16Level)
{
	uint8_t u8High;
	uint8_t u8Low;

    if((iChannel!=DDS_MAIN) && (iChannel!=DDS_LO))
    	return 0;
	if(iChannel==DDS_MAIN)
	{
		/* Enable channel 0 cmds and disable channel 1 cmds */
		WR_CSR(0x40|wire);

	}
	else
	{
	  	/* Enable channel 1 cmds and disable channel 0 cmds */
		WR_CSR(0x80|wire);
	}
	/* Adjust amplitude level */
	u8High=HI8(u16Level)|0x10;
	u8Low=LO8(u16Level);
	WR_ACR(0x00, u8High, u8Low);

	/* Enable both channel cmds */
	WR_CSR(0xc0|wire);

	/* Transfer registers */
	IOUpdatePulse();
	return 1;
}

/**
  * @brief  Enter or resumes from low power mode
  *
  * @param  u8Enable 	Action:
  *						@li true: resume from power down
  *						@li false: enter power down
  * @retval None
  */
#ifdef DDS_POWER_DOWN_ENABLE
void AD9958_Power_Control(uint8_t u8Enable)
{
	if (u8Enable)
	{
	  	/* Resume from power down */
		DDS_PWR_DOWN_LOW;
	 	WR_FR1(0xd0,0x00,0x00);
	}
	else
	{
		/* Enter power down */
	 	WR_FR1(0xd0,0x00,0xc0);
		DDS_PWR_DOWN_HIGH;
	}
}
#endif
/**
  * @brief  Write Channel Select Register register
  *
  * @param  u8Data	Reg value
  * @retval None
  */
static void WR_CSR(uint8_t u8Data)
{
	DDS_CS_LOW;

	dds_send(0x00);	/* CSR's address is 0 */
	dds_send(u8Data);

	DDS_CS_HIGH;
}

/**
  * @brief  Read Channel Select Register register
  *
  * @param  None
  * @retval Reg value
  */
static uint8_t RD_CSR(void)
{
	uint8_t u8Data;

	DDS_CS_LOW;

	dds_send(0x80);	/* CSR's address is 0 */
	u8Data = dds_rcv();

	DDS_CS_HIGH;

	return u8Data;
}
/**
  * @brief  Write Function Register 1 register
  *
  * @param  u8Data1	 MSB
  * @param  u8Data2
  * @param  u8Data3	 LSB
  * @retval None
  */
static void WR_FR1(uint8_t u8Data1,uint8_t u8Data2,uint8_t u8Data3)
{
	DDS_CS_LOW;

	dds_send(0x01);	/* FR1's address is 1 */
	dds_send(u8Data1);
	dds_send(u8Data2);
	dds_send(u8Data3);

	DDS_CS_HIGH;
}

/**
  * @brief  Write Function Register 2 register
  *
  * @param  u8Data1		MSB
  * @param  u8Data2	 	LSB
  * @retval None
  */
static void WR_FR2(uint8_t u8Data1,uint8_t u8Data2)
{
	DDS_CS_LOW;

	dds_send(0x02);	/* FR1's address is 2 */
	dds_send(u8Data1);
	dds_send(u8Data2);

	DDS_CS_HIGH;
}

/**
  * @brief  Write Channel Function Register1 register
  *
  * @param  u8Data1 	MSB
  * @param  u8Data2
  * @param  u8Data3 	LSB
  * @retval None
  */
static void WR_CFR(uint8_t u8Data1,uint8_t u8Data2,uint8_t u8Data3)
{
	DDS_CS_LOW;

	dds_send(0x03);	/* CFR's address is 3 */
	dds_send(u8Data1);
	dds_send(u8Data2);
	dds_send(u8Data3);

	DDS_CS_HIGH;
}
/**
  * @brief  Write Channel Frequency Tuning Word 01 register
  *
  * @param  u32ftWord 	freq tuning word
  * @retval None
  */
static void WR_CFTW0(uint32_t u32ftWord)
{
	DDS_CS_LOW;

	dds_send(0x04);	/* CFTW0's address is 4 */
	dds_send(((uint8_t *)(&u32ftWord))[3]);
	dds_send(((uint8_t *)(&u32ftWord))[2]);
	dds_send(((uint8_t *)(&u32ftWord))[1]);
	dds_send(((uint8_t *)(&u32ftWord))[0]);

	DDS_CS_HIGH;
}

/**
  * @brief  Write Channel Phase Offset Word 01
  *
  * @param  u32CpWord: phase offset word
  * @retval None
  */
static void WR_CPOW0(uint32_t u32CpWord)
{
	DDS_CS_LOW;

	dds_send(0x05);	/* CPW0's address is 5 */
	dds_send((((uint8_t *)(&u32CpWord))[1])&0x3F);
	dds_send(((uint8_t *)(&u32CpWord))[0]);

	DDS_CS_HIGH;
}

/**
  * @brief  Write Amplitude Control Register
  *
  * @param  u8Data1 	MSB
  * @param  u8Data2
  * @param  u8Data3 	LSB
  * @retval None
  */
static void WR_ACR(uint8_t u8Data1,uint8_t u8Data2,uint8_t u8Data3)
{
	DDS_CS_LOW;

	dds_send(0x06);	/* ACR's address is 3 */
	dds_send(u8Data1);
	dds_send(u8Data2);
	dds_send(u8Data3);

	DDS_CS_HIGH;
}

static void dds_send(uint8_t data)
{
    GPIO_InitTypeDef gpio;
    // GPIO_Pin_7 = MOSI
    gpio.GPIO_Pin = GPIO_Pin_7;
    gpio.GPIO_Mode = GPIO_Mode_OUT;
    gpio.GPIO_OType = GPIO_OType_PP;
    gpio.GPIO_Speed = GPIO_Speed_50MHz;
    gpio.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIOA, &gpio);

    for(uint8_t i=0; i<8; i++) {
        for(volatile int j=0; j<4; j++);
        GPIO_WriteBit(GPIOA, GPIO_Pin_7, (data & 0x80)? 1:0);
        data = data<<1;
        for(volatile int j=0; j<32; j++);
        GPIO_WriteBit(GPIOA, GPIO_Pin_5, 1);
        for(volatile int j=0; j<32; j++);
        GPIO_WriteBit(GPIOA, GPIO_Pin_5, 0);
    }
}

static uint8_t dds_rcv()
{
    GPIO_InitTypeDef gpio;
    // GPIO_Pin_7 = MOSI
    gpio.GPIO_Pin = GPIO_Pin_7;
    gpio.GPIO_Mode = GPIO_Mode_IN;
    gpio.GPIO_OType = GPIO_OType_PP;
    gpio.GPIO_Speed = GPIO_Speed_50MHz;
    gpio.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIOA, &gpio);

    uint8_t data = 0;
    for(uint8_t i=0; i<8; i++) {
        data = data<<1;
        for(volatile int j=0; j<32; j++);
        GPIO_WriteBit(GPIOA, GPIO_Pin_5, 1);
        data |= GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_7);
        for(volatile int j=0; j<32; j++);
        GPIO_WriteBit(GPIOA, GPIO_Pin_5, 0);
    }
    return data;
}

/**
  * @brief  Configures the different system clocks.
  * @param  None
  * @retval None
  */
static void RCC_Configuration(void)
{
	/* Enable peripheral clocks --------------------------------------------------*/
 	/* Enable SPI1 clock and GPIO clock for SPI1 */
  	//RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA |RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOE | RCC_APB2Periph_AFIO, ENABLE);

  	/* Enable SPI1 Periph clock */
  	//RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);

    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA | RCC_AHBPeriph_GPIOB, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);
}

/**
  * @brief  Configures the different GPIO ports.
  * @param  None
  * @retval None
  */
static void GPIO_Configuration(void)
{
    GPIO_InitTypeDef gpio;
    /* Configure SPI1 pins: SCK and MOSI */
    // GPIO_Pin_5 = SCK
    // GPIO_Pin_7 = MOSI
    gpio.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_7;
    gpio.GPIO_Mode = GPIO_Mode_OUT; //GPIO_Mode_IN
    gpio.GPIO_OType = GPIO_OType_PP;
    gpio.GPIO_Speed = GPIO_Speed_50MHz;
    gpio.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIOA, &gpio);

    gpio.GPIO_Pin = GPIO_Pin_4;
    gpio.GPIO_Mode = GPIO_Mode_OUT;
    gpio.GPIO_OType = GPIO_OType_PP;
    gpio.GPIO_Speed = GPIO_Speed_50MHz;
    gpio.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIOA, &gpio);

    //DDS_IO
	gpio.GPIO_Pin = GPIO_Pin_0;
	GPIO_Init(GPIOB, &gpio);

    //DDS_RESET
	gpio.GPIO_Pin = GPIO_Pin_1;
	GPIO_Init(GPIOB, &gpio);

#ifdef DDS_POWER_DOWN_ENABLE
    gpio.GPIO_Pin = GPIO_Pin_3;
    GPIO_Init(GPIOE, &gpio);
#endif
}

/**
  * @brief  Send pulse on IO_UPDT to transfer registers
  * @param  None
  * @retval None
  */
static void IOUpdatePulse(void)
{
/*
	volatile int iIdx;

	for (iIdx=0;iIdx<10;iIdx++)
		DDS_IO_UPD_LOW;
	DDS_IO_UPD_HIGH;
*/
  DDS_IO_UPD_LOW;
  Delay(1);
  DDS_IO_UPD_HIGH;
}

/**
  * @}
  */

/**
  * @}
  */

/************* (C) COPYRIGHT 2011-2013 Melchor Varela - EA4FRB *****END OF FILE****/

