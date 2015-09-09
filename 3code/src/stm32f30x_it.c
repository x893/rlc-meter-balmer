#include "main.h"

void NMI_Handler(void)		{	}
void SVC_Handler(void)		{	}
void DebugMon_Handler(void)	{	}
void PendSV_Handler(void)	{	}
void SysTick_Handler(void)	{	}

void HardFault_Handler(void)
{
	while (1)
	{
	}
}

void MemManage_Handler(void)
{
	while (1)
	{
	}
}

void BusFault_Handler(void)
{
	while (1)
	{
	}
}

void UsageFault_Handler(void)
{
	while (1)
	{
	}
}

#if defined (USB_INT_DEFAULT)
	void USB_LP_CAN1_RX0_IRQHandler(void)
#elif defined (USB_INT_REMAP)
	void USB_LP_IRQHandler(void)
#endif
{
	USB_Istr();
}

#if defined (USB_INT_DEFAULT)
	void USBWakeUp_IRQHandler(void)
#elif defined (USB_INT_REMAP)
	void USBWakeUp_RMP_IRQHandler(void)
#endif
{
	/* Initiate external resume sequence (1 step) */
	Resume(RESUME_EXTERNAL);
	EXTI_ClearITPendingBit(EXTI_Line18);
}

void DMA_ADC34_IRQHandler(void)
{
	ADC_VI_IRQHandler();
}

void TIM7_IRQHandler(void)
{
	Timer_IRQHandler();
}

void DMA2_Channel5_IRQHandler()
{
	DMA_ADC34_IRQHandler();
}
