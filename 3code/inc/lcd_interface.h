// balmer@inbox.ru RLC Meter 303
// 2013-2014

#include "pcd8544.h"

void LcdRepaint();

void printInt(int32_t value, uint8_t font);

extern int printD;

extern float Rre;
extern float Rim;

extern bool printRim;
extern bool isSerial;
extern bool valueIsC;
extern bool calculatedValues;
extern float valueL;
extern float valueC;
