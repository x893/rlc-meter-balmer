// balmer@inbox.ru RLC Meter 303
// 2013-2014

#ifndef __LCD_INTERFACE_H__
#define __LCD_INTERFACE_H__

#include "pcd8544.h"

#include <stdint.h>
#include <stdbool.h>

void LcdRepaint(void);
void LcdHello(void);

void printInt(int32_t value, LcdFontSize font);

extern int printD;
extern float printGradus;

extern float Rre;
extern float Rim;

extern bool printRim;
extern bool isSerial;
extern bool valueIsC;
extern bool calculatedValues;
extern float valueL;
extern float valueC;

#endif
