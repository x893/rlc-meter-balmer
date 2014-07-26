// balmer@inbox.ru RLC Meter 303
// 2013-2014

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
#include "vbat.h"

void InitLight();

void USB_Config(void)
{
  Set_USBClock();
  USB_Interrupts_Config();
  USB_Init();

  //while (bDeviceState != CONFIGURED) {}
}

void LcdHello()
{
  LcdClear();
  LcdGotoXYFont(3,3);
  LcdStr(FONT_2X, "RLC");

  LcdGotoXYFont(9, 3);
  LcdStr(FONT_1X, "meter");

  LcdGotoXYFont(3, 5);
  LcdStr(FONT_1X, "BALMER 303{");

  LcdUpdate();
}

int main(void)
{
  delay_init();
  Set_System();

  LcdInit();
  InitLight();

  VBatInit();

  USB_Config();

  DacInit();
  AdcInit();

  MCPInit();
  QuadEncInit();

  LcdHello();

  ClearCorrector();
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
