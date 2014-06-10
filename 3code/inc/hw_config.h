// balmer@inbox.ru RLC Meter 303
// 2013-2014

#ifndef __HW_CONFIG_H
#define __HW_CONFIG_H

#include "usb_type.h"
#ifdef STM32F30X
 #include "stm32f30x.h"
#endif /* STM32F30X */
#include <stdbool.h>

#define VIRTUAL_COM_PORT_DATA_SIZE              64


void Set_System(void);
void Set_USBClock(void);
void GPIO_AINConfig(void);
void Enter_LowPowerMode(void);
void Leave_LowPowerMode(void);
void USB_Interrupts_Config(void);
void USB_Cable_Config (FunctionalState NewState);
void Get_SerialNum(void);


/*
  bit 0 = PA
  bit 1 = PB
*/
void SetResistor(uint8_t idx);

//24000 == 3 KHz
#define LOW_PASS_PERIOD 24000 
/*
Add 3.3 nF capacitor to filter after DAC.
*/
void SetLowPassFilter(bool on);

#endif  /*__HW_CONFIG_H*/

