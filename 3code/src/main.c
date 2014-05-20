#include "main.h"

//#include <stdint.h>
#include "dac.h"
#include "adc.h"
#include "systick.h"
#include "mcp6s21.h"
#include "pcd8544.h"
#include "corrector.h"
#include "quadrature_encoder.h"
#include "lcd_interface.h"

void InitLight();

void USB_Config(void)
{
  Set_USBClock();
  USB_Interrupts_Config();
  USB_Init();

  //while (bDeviceState != CONFIGURED) {}
}

int main(void)
{
  delay_init();
  Set_System();

  CorrectorInit();
  LcdInit();
  InitLight();

  LcdClear();
  LcdGotoXYFont(1,1);
  LcdStr(FONT_1X, "Hello!");
  LcdUpdate();

  USB_Config();

  DacInit();
  AdcInit();

  MCPInit();
  QuadEncInit();

  while (1)
  {
    AdcQuant();
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
