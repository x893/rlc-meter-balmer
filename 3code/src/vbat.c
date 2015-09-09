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
	RCC_AHBPeriphClockCmd(ADC_BAT_RCC, ENABLE);

	/* Configure ADC Channel11 as analog input */
	GPIO_InitStructure.GPIO_Pin = ADC_BAT_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(ADC_BAT_PORT, &GPIO_InitStructure);

	ADC_StructInit(&ADC_InitStructure);

	/* Calibration procedure */
	ADC_VoltageRegulatorCmd(ADC_BAT, ENABLE);

	/* Insert delay equal to 10 µs */
	delay_us(10);

	ADC_SelectCalibrationMode(ADC_BAT, ADC_CalibrationMode_Single);
	ADC_StartCalibration(ADC_BAT);

	while (ADC_GetCalibrationStatus(ADC_BAT) != RESET)
		;
	//calibration_value = ADC_GetCalibrationValue(ADC_BAT);

	ADC_CommonInitStructure.ADC_Mode = ADC_Mode_Independent;
	ADC_CommonInitStructure.ADC_Clock = ADC_Clock_AsynClkMode;
	ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled;
	ADC_CommonInitStructure.ADC_DMAMode = ADC_DMAMode_OneShot;
	ADC_CommonInitStructure.ADC_TwoSamplingDelay = 0;
	ADC_CommonInit(ADC_BAT, &ADC_CommonInitStructure);

	ADC_InitStructure.ADC_ContinuousConvMode = ADC_ContinuousConvMode_Disable;
	ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;
	ADC_InitStructure.ADC_ExternalTrigConvEvent = ADC_ExternalTrigConvEvent_0;
	ADC_InitStructure.ADC_ExternalTrigEventEdge = ADC_ExternalTrigEventEdge_None;
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
	ADC_InitStructure.ADC_OverrunMode = ADC_OverrunMode_Disable;
	ADC_InitStructure.ADC_AutoInjMode = ADC_AutoInjec_Disable;
	ADC_InitStructure.ADC_NbrOfRegChannel = 1;
	ADC_Init(ADC_BAT, &ADC_InitStructure);

	/* ADC_BAT regular channel11 configuration */
	ADC_RegularChannelConfig(ADC_BAT, ADC_Channel_11, 1, ADC_SampleTime_19Cycles5);

	/* Enable ADC_BAT */
	ADC_Cmd(ADC_BAT, ENABLE);

	/* wait for ADRDY */
	while (!ADC_GetFlagStatus(ADC_BAT, ADC_FLAG_RDY))
		;

	ADC_StartConversion(ADC_BAT);
}

void VBatQuant()
{
	int32_t value;

	while (ADC_GetFlagStatus(ADC_BAT, ADC_FLAG_EOC) == RESET)	// Test EOC flag
		;

	value = ADC_GetConversionValue(ADC_BAT) - ADC_BAT_LOW;		// Get ADC_BAT converted data
	ADC_StartConversion(ADC_BAT);								// Start next conversion
	if (value < 0)
		value = 0;
	value = (value * 65) / (10 * (ADC_BAT_HIGH - ADC_BAT_LOW));
	LcdDrawBattery(value);
}
