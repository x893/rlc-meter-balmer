#include "hw_config.h"

void InitLight()
{
	//PE1 - pull down=light on
	GPIO_InitTypeDef GPIO_InitStruct;		
	// enable clock for used IO pins
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOE, ENABLE);
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_1;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_OD;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOE, &GPIO_InitStruct);
	
	GPIO_SetBits(GPIOE, GPIO_Pin_1);
}

void ToggleLight()
{
	uint8_t up = GPIO_ReadOutputDataBit(GPIOE, GPIO_Pin_1);
	GPIO_WriteBit(GPIOE, GPIO_Pin_1, up?Bit_RESET:Bit_SET);
}
