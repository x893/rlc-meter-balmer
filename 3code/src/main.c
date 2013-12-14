#include "main.h"

//#include <stdint.h>
#include "dac.h"
#include "adc.h"
#include "systick.h"



volatile float f = 12.0;
volatile float a = 10;
volatile float b = 15;

void USB_Config(void)
{
  Set_System();
  Set_USBClock();
  USB_Interrupts_Config();
  
  USB_Init();

  while (bDeviceState != CONFIGURED)
  {}
}

int main(void)
{
  delay_init();
  USB_Config();

  USB_SIL_Write(EP1_IN, (uint8_t *)"Hello", 5);
  SetEPTxValid(ENDP1); 

  DacInit();
  AdcInit();
  //DacSetFrequency(100000);
  //DacStart();

  while (1)
  {
    f = a*b;
  }
}

#ifdef USE_FULL_ASSERT
/*******************************************************************************
* Function Name  : assert_failed
* Description    : Reports the name of the source file and the source line number
*                  where the assert_param error has occurred.
* Input          : - file: pointer to the source file name
*                  - line: assert_param error line source number
* Output         : None
* Return         : None
*******************************************************************************/
void assert_failed(uint8_t* file, uint32_t line)
{
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {}
}
#endif

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
