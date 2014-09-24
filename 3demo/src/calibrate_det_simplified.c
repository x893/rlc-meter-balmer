/**
  ******************************************************************************
  * @file    DRV/SARK110/calibrate_det_simplified.c
  * @author  Melchor Varela - EA4FRB
  * @version V0.7.x
  * @date    12-December-2012
  * @brief	 Simplified module for base application.
  *			 It just provides the DDS amplitude setting
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

/* Includes ------------------------------------------------------------------*/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

//#include "device.h"
//#include "gui.h"
//#include "fonts.h"
#include "ad9958_drv.h"
//#include "dds_itfz.h"
#include "calibrate_det.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static const uint16_t gtu16MainAmpSetTableInit[] =	/* Signal amplitude settings table */
{
		0x1172,		/* 0-10M */
		0x116e,		/* 10-20M */
		0x1168,		/* 20-30M */
		0x1164,		/* 30-40M */
		0x1161,		/* 40-50M */
		0x1160,		/* 50-60M */
		0x1161,		/* 60-70M */
		0x1163,		/* 70-80M */
		0x1166,		/* 80-90M */
		0x116a,		/* 90-100M */
		0x116f,		/* 100-110M */
		0x1176,		/* 110-120M */
		0x117d,		/* 120-130M */
		0x1186,		/* 130-140M */
		0x1191,		/* 140-150M */
		0x119d,		/* 150-160M */
		0x11ac,		/* 160-170M */
		0x11bd,		/* 170-180M */
		0x11d3,		/* 180-190M */
		0x11ed,		/* 190-200M */
};
#define N_FREQ_STEPS		(sizeof(gtu16MainAmpSetTableInit)/sizeof(uint16_t))
#define FREQUENCY_DELTA		(MAX_FREQUENCY/(N_FREQ_STEPS-1))

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/**
  * @brief  Get the amplitude setting for the specified frequency.
  * This is obtained from a lookup table generated in a calib process.
  * This table is aimed to provide a constant amplitude with frequency.
  *
  * @param  u32Freq: Frequency
  * @retval Amplitude setting
  */
uint16_t GetAmplitudeWord (uint32_t u32Freq)
{
	int iI, iJ;
	float fPosition;
	uint16_t u16Word;

	iI = (int)(((double)u32Freq) / FREQUENCY_DELTA);	/* Frequency directly below ours */
	iJ = iI+1;											/* Frequency directly above ours */
	if (iJ >= (int)N_FREQ_STEPS)
		iJ = (int)N_FREQ_STEPS - 1;						/* In case we are close to MAX cal frequency */

	fPosition = (float) ((((double)u32Freq) / FREQUENCY_DELTA) - iI);	/* fractional position between i and j */
	u16Word = (uint16_t)(gtu16MainAmpSetTableInit[iI] + ((gtu16MainAmpSetTableInit[iJ] - gtu16MainAmpSetTableInit[iI]) * fPosition)); /* interpolate */

	return u16Word;
}

/**
  * @}
  */

/************* (C) COPYRIGHT 2011-2013 Melchor Varela - EA4FRB *****END OF FILE****/

