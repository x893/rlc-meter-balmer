// balmer@inbox.ru RLC Meter 303
// 2013-2014

#include "hw_config.h"
#include "systick.h"
#include "pcd8544.h"

void VBatInit()
{
	ADC_InitTypeDef       ADC_InitStructure;
	ADC_CommonInitTypeDef ADC_CommonInitStructure;
	GPIO_InitTypeDef      GPIO_InitStructure;
	GPIO_StructInit(&GPIO_InitStructure);
	/* ADC Channel configuration */
	RCC_ADCCLKConfig(RCC_ADC12PLLCLK_Div2);  
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_ADC12, ENABLE);

	/* GPIOC Periph clock enable */
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOC, ENABLE);

	/* Configure ADC Channel11 as analog input */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 ;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	ADC_StructInit(&ADC_InitStructure);

	/* Calibration procedure */  
	ADC_VoltageRegulatorCmd(ADC2, ENABLE);

	/* Insert delay equal to 10 µs */
	delay_us(10);

	ADC_SelectCalibrationMode(ADC2, ADC_CalibrationMode_Single);
	ADC_StartCalibration(ADC2);

	while(ADC_GetCalibrationStatus(ADC2) != RESET );
	//calibration_value = ADC_GetCalibrationValue(ADC2);

	ADC_CommonInitStructure.ADC_Mode = ADC_Mode_Independent;                                                                    
	ADC_CommonInitStructure.ADC_Clock = ADC_Clock_AsynClkMode;                    
	ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled;             
	ADC_CommonInitStructure.ADC_DMAMode = ADC_DMAMode_OneShot;                  
	ADC_CommonInitStructure.ADC_TwoSamplingDelay = 0;          
	ADC_CommonInit(ADC2, &ADC_CommonInitStructure);

	ADC_InitStructure.ADC_ContinuousConvMode = ADC_ContinuousConvMode_Disable;
	ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b; 
	ADC_InitStructure.ADC_ExternalTrigConvEvent = ADC_ExternalTrigConvEvent_0;         
	ADC_InitStructure.ADC_ExternalTrigEventEdge = ADC_ExternalTrigEventEdge_None;
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
	ADC_InitStructure.ADC_OverrunMode = ADC_OverrunMode_Disable;   
	ADC_InitStructure.ADC_AutoInjMode = ADC_AutoInjec_Disable;  
	ADC_InitStructure.ADC_NbrOfRegChannel = 1;
	ADC_Init(ADC2, &ADC_InitStructure);

	/* ADC2 regular channel11 configuration */ 
	ADC_RegularChannelConfig(ADC2, ADC_Channel_11, 1, ADC_SampleTime_19Cycles5);

	/* Enable ADC2 */
	ADC_Cmd(ADC2, ENABLE);

	/* wait for ADRDY */
	while(!ADC_GetFlagStatus(ADC2, ADC_FLAG_RDY));

	ADC_StartConversion(ADC2);
}

void VBatQuant()
{
	/* Start ADC2 Software Conversion */ 
 
	/* Test EOC flag */
	while(ADC_GetFlagStatus(ADC2, ADC_FLAG_EOC) == RESET);

	/* Get ADC2 converted data */
	int adcValue = ADC_GetConversionValue(ADC2);

	//Не задерживаем вывод на экран. Батарейка все равно медленно разряжается, поэтому показываем старое значение
	ADC_StartConversion(ADC2);

	const int Vup = 3080;//6.2 V
	const int Vdown = 2730;//5.5 V
	int value = 0;

	//Vup->6.5 Vdown->0
	value = adcValue-Vdown;
	if(value<0)
		value = 0;

	value = (value*65)/(10*(Vup-Vdown));

	//Draw battery
	byte x0 = 63, y0 = 0;
	LcdLine(x0, x0, y0+1, y0+7, PIXEL_ON );
	LcdLine(x0+1, x0+3, y0+1, y0+1, PIXEL_ON );
	LcdLine(x0+1, x0+3, y0+7, y0+7, PIXEL_ON );

	LcdLine(x0+3, x0+3, y0+1, y0+0, PIXEL_ON );
	LcdLine(x0+3, x0+3, y0+7, y0+8, PIXEL_ON );

	LcdLine(x0+3, x0+19, y0+0, y0+0, PIXEL_ON );
	LcdLine(x0+3, x0+19, y0+8, y0+8, PIXEL_ON );
	LcdLine(x0+20, x0+20, y0+0, y0+8, PIXEL_ON );

	if(value>5)
		LcdSingleBar( x0+2, y0+3+3, 3, 2, PIXEL_ON );

	for(byte i=0; i<5; i++)
	{
		if(value>4-i)
			LcdSingleBar( x0+5+i*3, y0+3+4, 5, 2, PIXEL_ON );
	}
 }
