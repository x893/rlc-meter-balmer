// balmer@inbox.ru RLC Meter 303
// 2013-2014

#include "hw_config.h"

void InitLight()
{
	// PE1 - pull down=light on
	GPIO_InitTypeDef GPIO_InitStruct;

	// enable clock for used IO pins
	RCC_AHBPeriphClockCmd(LCD_BL_RCC, ENABLE);
	GPIO_InitStruct.GPIO_Pin = LCD_BL_PIN;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_OD;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(LCD_BL_PORT, &GPIO_InitStruct);

	GPIO_SetBits(LCD_BL_PORT, LCD_BL_PIN);
}

void ToggleLight()
{
	uint8_t up = GPIO_ReadOutputDataBit(LCD_BL_PORT, LCD_BL_PIN);
	GPIO_WriteBit(LCD_BL_PORT, LCD_BL_PIN, up ? Bit_RESET : Bit_SET);
}
