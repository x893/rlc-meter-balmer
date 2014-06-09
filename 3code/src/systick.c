// balmer@inbox.ru RLC Meter 303
// 2013-2014

#include "systick.h"

static uint32_t delay_fac_us = 0;
static uint32_t delay_fac_ms = 0;
static FlagStatus  Status;


/*******************************************************************************
* Function Name  : SysTick_SetReload
* Description    : Sets SysTick Reload value.
* Input          : - Reload: SysTick Reload new value.
*                    This parameter must be a number between 1 and 0xFFFFFF.
* Output         : None
* Return         : None
*******************************************************************************/
void SysTick_SetReload(u32 Reload)
{
  /* Check the parameters */
  assert_param(IS_SYSTICK_RELOAD(Reload));

  SysTick->LOAD = Reload;
}

/*******************************************************************************
* Function Name  : SysTick_CounterCmd
* Description    : Enables or disables the SysTick counter.
* Input          : - SysTick_Counter: new state of the SysTick counter.
*                    This parameter can be one of the following values:
*                       - SysTick_Counter_Disable: Disable counter
*                       - SysTick_Counter_Enable: Enable counter
*                       - SysTick_Counter_Clear: Clear counter value to 0
* Output         : None
* Return         : None
*******************************************************************************/
void SysTick_CounterCmd(u32 SysTick_Counter)
{
  /* Check the parameters */
  assert_param(IS_SYSTICK_COUNTER(SysTick_Counter));

  if (SysTick_Counter == SysTick_Counter_Enable)
  {
    SysTick->CTRL |= SysTick_Counter_Enable;
  }
  else if (SysTick_Counter == SysTick_Counter_Disable) 
  {
    SysTick->CTRL &= SysTick_Counter_Disable;
  }
  else /* SysTick_Counter == SysTick_Counter_Clear */
  {
    SysTick->VAL = SysTick_Counter_Clear;
  }    
}

/*******************************************************************************
* Function Name  : SysTick_ITConfig
* Description    : Enables or disables the SysTick Interrupt.
* Input          : - NewState: new state of the SysTick Interrupt.
*                    This parameter can be: ENABLE or DISABLE.
* Output         : None
* Return         : None
*******************************************************************************/
void SysTick_ITConfig(FunctionalState NewState)
{
  /* Check the parameters */
  assert_param(IS_FUNCTIONAL_STATE(NewState));

  if (NewState != DISABLE)
  {
    SysTick->CTRL |= CTRL_TICKINT_Set;
  }
  else
  {
    SysTick->CTRL &= CTRL_TICKINT_Reset;
  }
}

/*******************************************************************************
* Function Name  : SysTick_GetCounter
* Description    : Gets SysTick counter value.
* Input          : None
* Output         : None
* Return         : SysTick current value
*******************************************************************************/
u32 SysTick_GetCounter(void)
{
  return(SysTick->VAL);
}

/*******************************************************************************
* Function Name  : SysTick_GetFlagStatus
* Description    : Checks whether the specified SysTick flag is set or not.
* Input          : - SysTick_FLAG: specifies the flag to check.
*                    This parameter can be one of the following values:
*                       - SysTick_FLAG_COUNT
*                       - SysTick_FLAG_SKEW
*                       - SysTick_FLAG_NOREF
* Output         : None
* Return         : None
*******************************************************************************/
FlagStatus SysTick_GetFlagStatus(u8 SysTick_FLAG)
{
  u32 statusreg = 0, tmp = 0 ;
  FlagStatus bitstatus = RESET;

  /* Check the parameters */
  assert_param(IS_SYSTICK_FLAG(SysTick_FLAG));

  /* Get the SysTick register index */
  tmp = SysTick_FLAG >> 3;

  if (tmp == 2) /* The flag to check is in CTRL register */
  {
    statusreg = SysTick->CTRL;
  }
  else          /* The flag to check is in CALIB register */
  {
    statusreg = SysTick->CALIB;
  }

  if ((statusreg & ((u32)1 << SysTick_FLAG)) != (u32)RESET)
  {
    bitstatus = SET;
  }
  else
  {
    bitstatus = RESET;
  }
  return bitstatus;
}


void delay_init(void)
{
    RCC_ClocksTypeDef RCC_ClocksStatus;

    RCC_GetClocksFreq(&RCC_ClocksStatus);
    SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK);
    SysTick_ITConfig(DISABLE);
    delay_fac_us = RCC_ClocksStatus.HCLK_Frequency / 1000000;
    delay_fac_ms = RCC_ClocksStatus.HCLK_Frequency / 1000;      
}
					
void delay_us(u32 Nus)
{ 
    SysTick_SetReload(delay_fac_us * Nus);
    SysTick_CounterCmd(SysTick_Counter_Clear);
    SysTick_CounterCmd(SysTick_Counter_Enable);
    do
    {
        Status = SysTick_GetFlagStatus(SysTick_FLAG_COUNT);
    }while (Status != SET);
    SysTick_CounterCmd(SysTick_Counter_Disable);
	SysTick_CounterCmd(SysTick_Counter_Clear);
}


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
