/**
  ******************************************************************************
  * @file    main.c
  * @author  MCD Application Team
  * @version V3.4.0
  * @date    29-June-2012
  * @brief   Virtual Com Port Demo main file
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2012 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */


/* Includes ------------------------------------------------------------------*/
#include "hw_config.h"
#include "usb_lib.h"
#include "usb_desc.h"
#include "usb_pwr.h"
#include "usb_init.h"
#include "SysTick/systick.h"
#include "voltage.h"
#include "dac.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Extern variables ----------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/*******************************************************************************
* Function Name  : main.
* Description    : Main routine.
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
/*
int main(void) {
  // Включаем порт А
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
  // Включаем ЦАП
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_DAC, ENABLE);
  // Включаем таймер 6
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6,ENABLE);
 
  // Настраиваем ногу ЦАПа
  GPIO_InitTypeDef GPIO_InitStructure;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  DAC_InitTypeDef DAC_InitStructure;
  DAC_InitStructure.DAC_Trigger = DAC_Trigger_None;
  DAC_InitStructure.DAC_WaveGeneration = DAC_WaveGeneration_None;
  DAC_InitStructure.DAC_OutputBuffer = DAC_OutputBuffer_Disable;
  //DAC_Init(DAC_Channel_1, &DAC_InitStructure);
  //DAC_Cmd(DAC_Channel_1, ENABLE);
  //DAC_SetChannel1Data(DAC_Align_12b_R, 1024);  

  // Настраиваем таймер так чтоб он тикал почаще
  TIM6->PSC = 0;
  TIM6->ARR = 500;
  TIM6->CR2=TIM_CR2_MMS_1; // Таймер будет источником событий для ЦАПа
  TIM6->CR1 |= TIM_CR1_CEN; // Начать отсчёт!
 
  // Включить DAC1
  DAC->CR |= DAC_CR_TEN1; // Преобразование по возникновению события ...
  DAC->CR &= ~DAC_CR_TSEL1; // ... от таймера 6
  //DAC->CR |= DAC_CR_WAVE1_0; // Генерация шума 
  DAC->CR |= DAC_CR_WAVE1_1; // Генерация сигнала треугольной формы 
  DAC->CR |= DAC_CR_MAMP1; // Максимальная амплитуда 
  DAC->CR |= DAC_CR_EN1; // Включить ЦАП1 

  // Бесконечный цикл 
  while (1)
  {
  }
}
*/

int main(void)
{
  //uint32_t i;
  //Set_System();
  //Set_USBClock();
  //USB_Interrupts_Config();
  //USB_Init();
  delay_init();
  DacInit();

  //USB_SetLeds('1');

  while (1)
  {
    //USB_SetLeds('1');
    //delay_ms(1000);
    //USB_SetLeds('0');
    //delay_ms(1000);
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
