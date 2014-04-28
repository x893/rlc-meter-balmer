#ifndef _FORMAT_PRINT_H_
#define _FORMAT_PRINT_H_

void printInt(int32_t value, uint8_t font);
void printIntFixed(int32_t value, uint8_t font, uint8_t aMinDigits, uint8_t aFixedPoint);

void printC(float aValue, uint8_t font);
void printR(float aValue, uint8_t font);
void printV(float aValue);
void printT(float aValue);
void printF(float aValue);
void printL(float aValue, uint8_t font);

void printRX2(float aValue, uint8_t y);

#endif//_FORMAT_PRINT_H_