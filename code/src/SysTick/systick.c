/****************************************Copyright (c)****************************************************
**                                      
**                                 http://www.powermcu.com
**
**--------------File Info---------------------------------------------------------------------------------
** File name:               systick.c
** Descriptions:            Ê¹ÓÃSysTickµÄÆÕÍ¨¼ÆÊýÄ£Ê½¶ÔÑÓ³Ù½øÐÐ¹ÜÀí
**
**--------------------------------------------------------------------------------------------------------
** Created by:              AVRman
** Created date:            2010-10-30
** Version:                 v1.0
** Descriptions:            The original version
**
**--------------------------------------------------------------------------------------------------------
** Modified by:             
** Modified date:           
** Version:                 
** Descriptions:            
**
*********************************************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "systick.h"
#include "stm32f10x_systick.h"

/* Private variables ---------------------------------------------------------*/	 
static uint32_t delay_fac_us = 0;
static uint32_t delay_fac_ms = 0;
static FlagStatus  Status;


/*******************************************************************************
* Function Name  : delay_init
* Description    : ³õÊ¼»¯ÑÓ³Ùº¯Êý 
* Input          : None
* Output         : None
* Return         : None
* Attention		 : SYSTICKµÄÊ±ÖÓ¹Ì¶¨ÎªHCLKÊ±ÖÓµÄ1/8
*******************************************************************************/
void delay_init(void)
{
    RCC_ClocksTypeDef RCC_ClocksStatus;

    RCC_GetClocksFreq(&RCC_ClocksStatus);
    SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK);
	SysTick_ITConfig(DISABLE);
    delay_fac_us = RCC_ClocksStatus.HCLK_Frequency / 1000000;
    delay_fac_ms = RCC_ClocksStatus.HCLK_Frequency / 1000;      
}
					
/*******************************************************************************
* Function Name  : delay_us
* Description    : ³õÊ¼»¯ÑÓ³Ùº¯Êý 
* Input          : - Nus: ÑÓÊ±us
* Output         : None
* Return         : None
* Attention		 : 0xffffff / (SYSCLK / 1000000), SYSCLK=72 MHz, Nus = 233016
*******************************************************************************/            
void delay_us(u32 Nus)
{ 
    SysTick_SetReload(delay_fac_us * Nus);          /* Ê±¼ä¼ÓÔØ */
    SysTick_CounterCmd(SysTick_Counter_Clear);		/* Çå¿Õ¼ÆÊýÆ÷ */
    SysTick_CounterCmd(SysTick_Counter_Enable);		/* ¿ªÊ¼µ¹Êý */ 	
    do
    {
        Status = SysTick_GetFlagStatus(SysTick_FLAG_COUNT);
    }while (Status != SET);							/* µÈ´ýÊ±¼äµ½´ï */
    SysTick_CounterCmd(SysTick_Counter_Disable);    /* ¹Ø±Õ¼ÆÊýÆ÷ */
	SysTick_CounterCmd(SysTick_Counter_Clear);	    /* Çå¿Õ¼ÆÊýÆ÷ */   
}


/*******************************************************************************
* Function Name  : delay_ms
* Description    : ³õÊ¼»¯ÑÓ³Ùº¯Êý 
* Input          : - nms: ÑÓÊ±ms
* Output         : None
* Return         : None
* Attention		 : nms<=0xffffff*1000/SYSCLK, SYSCLK=72 MHz, nms<=233 
*******************************************************************************/  
void delay_ms(uint16_t nms)
{   
    uint32_t temp = delay_fac_ms * nms;

    if (temp > 0x00ffffff)
    {
        temp = 0x00ffffff;
    }
    SysTick_SetReload(temp);
    SysTick_CounterCmd(SysTick_Counter_Clear);
    SysTick_CounterCmd(SysTick_Counter_Enable);
    do
    {
        Status = SysTick_GetFlagStatus(SysTick_FLAG_COUNT);
    }while (Status != SET);
    SysTick_CounterCmd(SysTick_Counter_Disable);
	SysTick_CounterCmd(SysTick_Counter_Clear);
}

/*********************************************************************************************************
      END FILE
*********************************************************************************************************/
















