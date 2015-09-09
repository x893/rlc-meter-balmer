// balmer@inbox.ru RLC Meter 303
// 2013-2014

#ifndef _FORMAT_PRINT_H_
#define _FORMAT_PRINT_H_

#include <stdint.h>

void printInt(int32_t value, LcdFontSize font);
void printIntFixed(int32_t value, LcdFontSize font, uint8_t aMinDigits, uint8_t aFixedPoint);

void printC(float aValue, LcdFontSize font);
void printR(float aValue, LcdFontSize font);
void printV(float aValue);
void printT(float aValue);
void printF(float aValue);
void printL(float aValue, LcdFontSize font);

void printRX2(float aValue, uint8_t y);
void printLX2(float aValue, uint8_t y);
void printCX2(float aValue, uint8_t y);

#endif//_FORMAT_PRINT_H_
