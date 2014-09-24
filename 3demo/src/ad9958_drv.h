/**
  ******************************************************************************
  * @file    DRV/INC/ad9958_driver.h
  * @author  Melchor Varela - EA4FRB
  * @version V0.7.x
  * @date    12-December-2012
  * @brief   DDS AD9958 Driver
  ******************************************************************************
  * @copy
  *
  *  This file is a part of the "SARK110 Antenna Vector Impedance Analyzer" firmware
  *
  *  "SARK110 Antenna Vector Impedance Analyzer firmware" is free software: you can redistribute it
  *  and/or modify it under the terms of the GNU General Public License as
  *  published by the Free Software Foundation, either version 3 of the License,
  *  or (at your option) any later version.
  *
  *  "SARK110 Antenna Vector Impedance Analyzer firmware" is distributed in the hope that it will be
  *  useful,  but WITHOUT ANY WARRANTY; without even the implied warranty of
  *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  *  GNU General Public License for more details.
  *
  *  You should have received a copy of the GNU General Public License
  *  along with "SARK110 Antenna Vector Impedance Analyzer" firmware.  If not,
  *  see <http://www.gnu.org/licenses/>.
  *
  * <h2><center>&copy; COPYRIGHT 2011-2013 Melchor Varela - EA4FRB </center></h2>
  *  Melchor Varela, Madrid, Spain.
  *  melchor.varela@gmail.com
  */

/** @addtogroup DRV
  * @{
  */

/** @addtogroup AD9958
  * @{
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __AD9958_DRV_H__
#define __AD9958_DRV_H__

/* Includes ------------------------------------------------------------------*/
//#include <device.h>
#include <stdint.h>
#include <stdbool.h>

#define MIN_FREQUENCY   100000    /*!< Minimum DDS frequency: 100Khz */
#define MAX_FREQUENCY   230000000 /*!< Maximum DDS frequency: 230Mhz */

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/

/** @defgroup AD9958_Exported_Constants
  * @{
  */

#define DDS_MAIN	0					/*!< Main channel */
#define DDS_LO		1					/*!< Local channel */

/**
  * @}
  */

/* Exported macro ------------------------------------------------------------*/

/* Exported functions ------------------------------------------------------- */

/** @defgroup AD9958_Exported_Functions
  * @{
  */

/**
  * @brief  Initializes the AD9958 device.
  *
  * @retval None
  */
extern void AD9958_Init(void);

/**
  * @brief  Sets the DDS frequency
  *
  * @param  iChannel 	DDS Channel:
  *						@li DDS_MAIN
  *						@li DDS_LO
  * @param  dfFreq 		Frequency in Hz
  * @retval iError		Error code:
  *						@li 1: OK
  *						@li 0: Wrong argument
  */
extern int AD9958_Set_Frequency(int iChannel, double dfFreq);

/**
  * @brief  Test the communication with the DDS
  *
  * @retval bError		Error code:
  *						@li TRUE: OK
  *						@li FALSE: failure
 */
extern bool AD9958_Test(void);

/**
  * @brief  Enter or resumes from low power mode
  *
  * @param  u8Enable 	Action:
  *						@li TRUE: resume from power down
  *						@li FALSE: enter power down
  * @retval None
  */
extern void AD9958_Power_Control(uint8_t u8Enable);

/**
  * @brief  Sets the amplitude level for the selected channel
  *
  * @param  iChannel 	DDS Channel:
  *						@li DDS_MAIN
  *						@li DDS_LO
  * @param  u16Level 	Amplitude word
  * @retval iError		Error code:
  *						@li 1: OK
  *						@li 0: Wrong argument
  */
extern int AD9958_Set_Level(int iChannel,uint16_t u16Level);


/**
  * @}
  */

#endif	 /* __AD9958_DRV_H__ */

/**
  * @}
  */

/**
  * @}
  */

/************* (C) COPYRIGHT 2011-2013 Melchor Varela - EA4FRB *****END OF FILE****/

