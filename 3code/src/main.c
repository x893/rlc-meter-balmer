// balmer@inbox.ru RLC Meter 303
// 2013-2014

#include "main.h"
#include "dac.h"
#include "adc.h"
#include "systick.h"
#include "mcp6s21.h"
#include "pcd8544.h"
#include "corrector.h"
#include "quadrature_encoder.h"
#include "lcd_interface.h"
#include "vbat.h"

//	LCD primitives (from PCD8544.c)
//	-------------------------------
__weak uint8_t LcdGotoXYFont( uint8_t x, uint8_t y )
{
	return OK;
}
__weak uint8_t LcdSingleBar( uint8_t baseX, uint8_t baseY, uint8_t height, uint8_t width, LcdPixelMode mode )
{
	return OK;
}
__weak uint8_t LcdStr( LcdFontSize size, const char* dataArray)
{
	return OK;
}
__weak void LcdClear( void ) { }
__weak void LcdUpdate( void ) { }
__weak void LcdInit( void ) { }
__weak uint8_t LcdLine( uint8_t x1, uint8_t x2, uint8_t y1, uint8_t y2, LcdPixelMode mode )
{
	return OK;
}
//	-------------------------------
void InitLight(void);

void USB_Config(void)
{
	Set_USBClock();
	USB_Interrupts_Config();
	USB_Init();
	// while (bDeviceState != CONFIGURED) {}
}

int main(void)
{
	delay_init();
	Set_System();

	LcdInit();
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
* Function Name	: assert_failed
* Description		: Reports the name of the source file and the source line number
*									where the assert_param error has occurred.
* Input					: - file: pointer to the source file name
*									- line: assert_param error line source number
* Output				 : None
* Return				 : None
*******************************************************************************/
void assert_failed(uint8_t* file, uint32_t line)
{
	/* User can add his own implementation to report the file name and line number,
		 ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

	/* Infinite loop */
	while (1)
	{
	}
}
#endif

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
