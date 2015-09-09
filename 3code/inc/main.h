#ifndef __MAIN_H__
#define __MAIN_H__

#include <stdio.h>
#include <math.h>
#include "stm32f30x.h"
#include "usb_lib.h"
#include "hw_config.h"
#include "usb_pwr.h"
#include "usb_istr.h"
#include "stm32f30x_it.h"
#include "usb_desc.h"

void USB_Config (void);

//	116 Kb to program
//	2 Kb = 0x800
#define FLASH_PROGRAM_MAX_SIZE 0x1D000
#define FLASH_START_ARRAY	(FLASH_BASE + FLASH_PROGRAM_MAX_SIZE)

#if defined(__GNUC__)
	#define __weak
#endif

#endif
