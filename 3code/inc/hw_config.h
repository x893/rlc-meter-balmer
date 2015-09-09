// balmer@inbox.ru RLC Meter 303
// 2013-2014

#ifndef __HW_CONFIG_H
#define __HW_CONFIG_H

#include <stdbool.h>
#include <stdint.h>
#include "usb_type.h"

#if defined( STM32F30X ) || defined( STM32F303xC )
	#include "stm32f30x.h"
#endif /* STM32F30X */

#define NELEMENTS(a)		(sizeof(a) / sizeof(*(a)))
#define VIRTUAL_COM_PORT_DATA_SIZE	64

void Set_System				(void);
void Set_USBClock			(void);
void GPIO_AINConfig			(void);
void Enter_LowPowerMode		(void);
void Leave_LowPowerMode		(void);
void USB_Interrupts_Config	(void);
void USB_Cable_Config		(FunctionalState NewState);
void Get_SerialNum			(void);

#define USB_ON_PORT			GPIOC
#define USB_ON_PIN			GPIO_Pin_6

#define AN_SW_PORT			GPIOB
#define AN_SW_PIN			GPIO_Pin_10

#define RAB_PORT			GPIOD
#define RA_PIN				GPIO_Pin_1
#define RB_PIN				GPIO_Pin_0

#define LCD_BL_PORT			GPIOE
#define LCD_BL_PIN			GPIO_Pin_1
#define LCD_BL_RCC			RCC_AHBPeriph_GPIOE

#define LCD_RST_PORT		GPIOD
#define LCD_RST_PIN			GPIO_Pin_2

#define LCD_DC_PORT			GPIOD
#define LCD_DC_PIN			GPIO_Pin_4

#define LCD_CE_PORT			GPIOD
#define LCD_CE_PIN			GPIO_Pin_3

#define ADC_BAT_PORT		GPIOC
#define ADC_BAT_PIN			GPIO_Pin_5
#define ADC_BAT_RCC			RCC_AHBPeriph_GPIOC
#define ADC_BAT				ADC2
#define ADC_BAT_HIGH		3080	//6.2 V
#define ADC_BAT_LOW			2730	//5.5 V

#define V_ADC_PORT			GPIOB
#define V_ADC_PIN			GPIO_Pin_1
#define V_ADC_RCC			RCC_AHBPeriph_GPIOB

#define I_ADC_PORT			GPIOE
#define I_ADC_PIN			GPIO_Pin_14
#define I_ADC_RCC			RCC_AHBPeriph_GPIOE

#define GENERATOR_PORT		GPIOA
#define GENERATOR_PIN		GPIO_Pin_4
#define GENERATOR_PIN_CLK	RCC_AHBPeriph_GPIOA
#define GENERATOR_TIMER		TIM2
#define GENERATOR_TIMER_CLK	RCC_APB1Periph_TIM2

#define SPI1_PORT			GPIOB
#define SPI1_RCC			RCC_AHBPeriph_GPIOB
#define SPI1_MOSI_PIN		GPIO_Pin_5
#define SPI1_CLK_PIN		GPIO_Pin_3

#define SPI_CS_PORT			GPIOD
#define SPI_CS_RCC			RCC_AHBPeriph_GPIOD
#define SPI_CS_I_PIN		GPIO_Pin_6
#define SPI_CS_V_PIN		GPIO_Pin_7

#define DEF_INTERFACE_BUTTONS

#define Codeur_A			GPIO_Pin_6
#define Codeur_A_SOURCE		GPIO_PinSource6
#define Codeur_B			GPIO_Pin_7
#define Codeur_B_SOURCE		GPIO_PinSource7
#define Codeur_GPIO			GPIOA
#define Codeur_RCC			RCC_AHBPeriph_GPIOA
#define Codeur_AF			GPIO_AF_2
 
#define Codeur_TIMER		TIM3
#define Codeur_TIMER_RCC	RCC_APB1Periph_TIM3

#define BUTTON_PIN 			GPIO_Pin_5
#define BUTTON_GPIO			GPIOA

/*
  bit 0 = RA
  bit 1 = RB
*/
void SetResistor(uint8_t idx);

//24000 == 3 KHz
#define LOW_PASS_PERIOD 24000 
/*
Add 3.3 nF capacitor to filter after DAC.
*/
void SetLowPassFilter(bool on);

#endif  /*__HW_CONFIG_H*/

