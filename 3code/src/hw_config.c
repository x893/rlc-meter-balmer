#include "main.h"

ErrorStatus HSEStartUpStatus;
EXTI_InitTypeDef EXTI_InitStructure;

/* Private function prototypes -----------------------------------------------*/
static void IntToUnicode (uint32_t value , uint8_t *pbuf , uint8_t len);
/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Configures Main system clocks & power.
  * @param  None
  * @retval None
  */
void Set_System(void)
{
  GPIO_InitTypeDef  GPIO_InitStructure;
  
  /*!< At this stage the microcontroller clock setting is already configured, 
       this is done through SystemInit() function which is called from startup
       file (startup_stm32f30x.s) before to branch to application main.
       To reconfigure the default setting of SystemInit() function, refer to
       system_stm32f30x.c file
     */ 

 /* Enable the PWR clock */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);

  /* Enable the SYSCFG module clock (used for the USB disconnect feature) */
  
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);

  /* Enable the USB disconnect GPIO clock */
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIO_DISCONNECT, ENABLE);

 /*Set PA11,12 as IN - USB_DM,DP*/
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11 | GPIO_Pin_12;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
      
  /*SET PA11,12 for USB: USB_DM,DP*/
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource11, GPIO_AF_14);
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource12, GPIO_AF_14);
  
#if defined(USB_USE_EXTERNAL_PULLUP)
  /* Enable the USB disconnect GPIO clock */
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIO_DISCONNECT, ENABLE);

  /* USB_DISCONNECT used as USB pull-up */
  GPIO_InitStructure.GPIO_Pin = USB_DISCONNECT_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(USB_DISCONNECT, &GPIO_InitStructure);  
#endif /* USB_USE_EXTERNAL_PULLUP */
   
  /* Configure the EXTI line 18 connected internally to the USB IP */
  EXTI_ClearITPendingBit(EXTI_Line18);
  EXTI_InitStructure.EXTI_Line = EXTI_Line18; /*USB resume from suspend mode*/
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_InitStructure);
 }

/**
  * @brief  Configures USB Clock input (48MHz).
  * @param  None
  * @retval None
  */
void Set_USBClock(void)
{
  /* USBCLK = PLLCLK = 48 MHz */
  RCC_USBCLKConfig(RCC_USBCLKSource_PLLCLK_1Div5);
  
  /* Enable USB clock */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_USB, ENABLE);
}

/**
  * @brief  Configures all IOs as AIN to reduce the power consumption.
  * @param  None
  * @retval None
  */
void GPIO_AINConfig(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
 
  /* Configure all GPIO port pins in Analog Input mode (floating input trigger OFF) */

  
  /* Enable all GPIOs Clock*/
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_ALLGPIO, ENABLE);  
  
 /* Configure all GPIO port pins in Analog Input mode (floating input trigger OFF) */

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_All;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
  GPIO_Init(GPIOC, &GPIO_InitStructure);
  GPIO_Init(GPIOD, &GPIO_InitStructure);
  GPIO_Init(GPIOE, &GPIO_InitStructure);

  /* Disable all GPIOs Clock*/
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_ALLGPIO, DISABLE); 
}

/**
  * @brief  Power-off system clocks and power while entering suspend mode.
  * @param  None
  * @retval None
  */
void Enter_LowPowerMode(void)
{
  /* Set the device state to suspend */
  bDeviceState = SUSPENDED;
} 

/**
  * @brief  Restores system clocks and power while exiting suspend mode.
  * @param  None
  * @retval None
  */
void Leave_LowPowerMode(void)
{
  DEVICE_INFO *pInfo = &Device_Info;

  /* Enable HSE */
  RCC_HSEConfig(RCC_HSE_ON);

  /* Wait till HSE is ready */
  HSEStartUpStatus = RCC_WaitForHSEStartUp();


  /* Wait till HSE is ready */
  while (RCC_GetFlagStatus(RCC_FLAG_HSERDY) == RESET)
  {}
   
  /* Enable PLL1 */
  RCC_PLLCmd(ENABLE);

  /* Wait till PLL1 is ready */
  while (RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET)
  {}

  /* Select PLL as system clock source */
  RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);

  /* Wait till PLL is used as system clock source */ 
  while (RCC_GetSYSCLKSource() != 0x08)
  {}  

 /* Set the device state to the correct state */
  if (pInfo->Current_Configuration != 0)
  {
    /* Device configured */
    bDeviceState = CONFIGURED;
  }
  else
  {
    bDeviceState = ATTACHED;
  }
}

/**
  * @brief  Configures the USB interrupts.
  * @param  None
  * @retval None
  */
void USB_Interrupts_Config(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;

  /* 2 bit for pre-emption priority, 2 bits for subpriority */
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);

  /* Enable the USB interrupt */
#if defined (USB_INT_DEFAULT)
  NVIC_InitStructure.NVIC_IRQChannel = USB_LP_CAN1_RX0_IRQn;
#endif
#if defined (USB_INT_REMAP)
  NVIC_InitStructure.NVIC_IRQChannel = USB_LP_IRQn;
#endif 
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
  
  /* Enable the USB Wake-up interrupt */
#if defined (USB_INT_DEFAULT)
  NVIC_InitStructure.NVIC_IRQChannel = USBWakeUp_IRQn;
#endif
#if defined (USB_INT_REMAP)  
  NVIC_InitStructure.NVIC_IRQChannel = USBWakeUp_RMP_IRQn;
#endif  
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);      
}

/**
  * @brief  Software Connection/Disconnection of USB Cable.
  * @param  None
  * @retval None
  */
void USB_Cable_Config (FunctionalState NewState)
{
  if (NewState != DISABLE)
  {
    //balmer GPIO_ResetBits(USB_DISCONNECT, USB_DISCONNECT_PIN);
  }
  else
  {
    //balmer GPIO_SetBits(USB_DISCONNECT, USB_DISCONNECT_PIN);
  }
}


/**
  * @brief  Create the serial number string descriptor.
  * @param  None.
  * @retval None
  */
void Get_SerialNum(void)
{
  uint32_t Device_Serial0, Device_Serial1, Device_Serial2;
 
  Device_Serial0 = *(__IO uint32_t*)(0x1FFFF7E8);
  Device_Serial1 = *(__IO uint32_t*)(0x1FFFF7EC);
  Device_Serial2 = *(__IO uint32_t*)(0x1FFFF7F0);
  
  Device_Serial0 += Device_Serial2;

  if (Device_Serial0 != 0)
  {
    IntToUnicode (Device_Serial0, &Joystick_StringSerial[2] , 8);
    IntToUnicode (Device_Serial1, &Joystick_StringSerial[18], 4);
  }
}

/**
  * @brief  Convert Hex 32Bits value into char.
  * @param  value: Data to be converted.
  * @param  pbuf: pointer to buffer.  
  * @param  len: Data length.   
  * @retval None
  */
static void IntToUnicode (uint32_t value , uint8_t *pbuf , uint8_t len)
{
  uint8_t idx = 0;
  
  for( idx = 0 ; idx < len ; idx ++)
  {
    if( ((value >> 28)) < 0xA )
    {
      pbuf[ 2* idx] = (value >> 28) + '0';
    }
    else
    {
      pbuf[2* idx] = (value >> 28) + 'A' - 10; 
    }
    
    value = value << 4;
    
    pbuf[ 2* idx + 1] = 0;
  }
}
