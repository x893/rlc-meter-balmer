/**
  ******************************************************************************
  * @file    DRV/SARK110/INC/calibrate_det.h
  * @author  Melchor Varela - EA4FRB
  * @version V0.7.x
  * @date    12-December-2012
  * @brief   Detector calibration procedures
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __CALIBRATE_DET_H__
#define __CALIBRATE_DET_H__

/* Includes ------------------------------------------------------------------*/
//#include "device.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
extern uint16_t GetAmplitudeWord (uint32_t u32Freq);

#endif	 /* __CALIBRATE_DET_H__ */

/**
  * @}
  */

/************* (C) COPYRIGHT 2011-2013 Melchor Varela - EA4FRB *****END OF FILE****/

