#include "stm32f30x.h"
#include <stdio.h>
#include "usb_lib.h"
#include "hw_config.h"
#include "usb_pwr.h"
#include "platform_config.h"
#include "math.h"
#include "usb_istr.h"
#include "stm32f30x_it.h"
#include "usb_desc.h"


void USB_Config (void);

//116 Kb to program
//Должно быть кратно 2 Kb = 0x800
#define FLASH_PROGRAM_MAX_SIZE 0x1D000
#define FLASH_START_ARRAY  (FLASH_BASE+FLASH_PROGRAM_MAX_SIZE)

