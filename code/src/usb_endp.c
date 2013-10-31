/**
  ******************************************************************************
  * @file    usb_endp.c
  * @author  MCD Application Team
  * @version V3.4.0
  * @date    29-June-2012
  * @brief   Endpoint routines
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
#include "usb_lib.h"
#include "usb_desc.h"
#include "usb_mem.h"
#include "hw_config.h"
#include "usb_istr.h"
#include "usb_pwr.h"
#include "voltage.h"
#include "sound.h"
#include <string.h>

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/

/* Interval between sending IN packets in frame number (1 frame = 1ms) */
#define VCOMPORT_IN_FRAME_INTERVAL             5

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
uint8_t USB_Rx_Buffer[VIRTUAL_COM_PORT_DATA_SIZE];
extern  uint8_t USART_Rx_Buffer[];
extern uint32_t USART_Rx_ptr_out;
extern uint32_t USART_Rx_length;
extern uint8_t  USB_Tx_State;

static char USB_first_SOF = 1;

void USBAdd(uint8_t* data, uint32_t size)
{
  uint32_t i;
  if(USART_Rx_length+size>VIRTUAL_COM_PORT_DATA_SIZE)
    size = VIRTUAL_COM_PORT_DATA_SIZE - USART_Rx_length;
  for (i=0; i<size; i++)
    USART_Rx_Buffer[USART_Rx_length++] = data[i];
}

void USBAddStr(char* data)
{
  USBAdd((uint8_t*)data, strlen(data));
}

void USBAdd8(uint8_t data)
{
  USBAdd((uint8_t*)&data, sizeof(data));
}

void USBAdd16(uint16_t data)
{
  USBAdd((uint8_t*)&data, sizeof(data));
}

void USBAdd32(uint32_t data)
{
  USBAdd((uint8_t*)&data, sizeof(data));
}

void USBSend(void)
{
  USB_SIL_Write(EP1_IN, USART_Rx_Buffer, USART_Rx_length);
  #ifndef USE_STM3210C_EVAL
  SetEPTxValid(ENDP1); 
  #endif
  USART_Rx_length = 0;
}

/*******************************************************************************
* Function Name  : EP1_IN_Callback
* Description    :
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void EP1_IN_Callback (void)
{
  if(adc_get_result_command)
    SendConversionResult();
  if(sound_command)
    SoundFillUsb();
  USBSend();
}

/*******************************************************************************
* Function Name  : EP3_OUT_Callback
* Description    :
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void EP3_OUT_Callback(void)
{
  uint16_t USB_Rx_Cnt;
//  uint16_t i;
  
  /* Get the received data buffer and update the counter */
  USB_Rx_Cnt = USB_SIL_Read(EP3_OUT, USB_Rx_Buffer);
  
  /* USB data will be immediately processed, this allow next USB traffic being 
  NAKed till the end of the USART Xfer */
  USBCommandReceive(USB_Rx_Buffer, USB_Rx_Cnt);
/*
  //Balmer test code 
  for (i=0; i<USB_Rx_Cnt; i++)
    USART_Rx_Buffer[USART_Rx_length++] = USB_Rx_Buffer[i];
  USBSend();
  
  for (i=0; i<USB_Rx_Cnt; i++) {
    USB_SetLeds(USB_Rx_Buffer[i]);
  }
*/  

#ifndef STM32F10X_CL
  /* Enable the receive of data on EP3 */
  SetEPRxValid(ENDP3);
#endif /* STM32F10X_CL */
}


/*******************************************************************************
* Function Name  : SOF_Callback / INTR_SOFINTR_Callback
* Description    :
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
#ifdef STM32F10X_CL
void INTR_SOFINTR_Callback(void)
#else
void SOF_Callback(void)
#endif /* STM32F10X_CL */
{
  static uint32_t FrameCount = 0;
  
  if(bDeviceState == CONFIGURED)
  {
    if (FrameCount++ == VCOMPORT_IN_FRAME_INTERVAL)
    {
      /* Reset the frame counter */
      FrameCount = 0;
      
      /* Check the data to be sent through IN pipe */
      //SetEPTxValid(ENDP1);
      //Handle_USBAsynchXfer();
      if(USB_first_SOF)
      {
        //USB_SIL_Write(EP1_IN, USART_Rx_Buffer, USART_Rx_length);//Balmer test code
        //#ifndef USE_STM3210C_EVAL
        //SetEPTxValid(ENDP1); 
        //#endif
        USB_first_SOF = 0;
        //USART_Rx_length = 0;
      }

    }
  }  
}
