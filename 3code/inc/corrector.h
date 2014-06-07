#ifndef _CORRECTOR_H_
#define _CORRECTOR_H_

#include "calc_rc.h"

#define CORRECTOR2X_RESISTOR_COUNT 3
#define CORRECTOR2X_GAIN_COUNT 3
#define CORRECTOR_OPEN_SHORT_GAIN_COUNT 6
#define PREDEFINED_PERIODS_COUNT 4

typedef struct Zm2x
{
	complexf Zm1;//measured minimal value
	complexf Zm2;//measured mmaximal value
} Zm2x;

typedef struct ZmOpen
{
	complexf Zstdm;//measured load 100 КОм
	complexf Zom;//measured open fixtures
} ZmOpen;

typedef struct ZmShort
{
	complexf Zstdm;//measured load
	complexf Zsm;//measured short
} ZmShort;

typedef struct CoeffCorrector2x
{
	Zm2x Zm[CORRECTOR2X_GAIN_COUNT];
	float Z1;//real minimal value
	float Z2;//real maximal value
} CoeffCorrector2x;

typedef struct CoeffCorrectorOpen
{
	ZmOpen Zm[CORRECTOR_OPEN_SHORT_GAIN_COUNT];
	float R;//precize 100 КОм real value
	float C;//capacitance load
	uint32_t maxGainIndex;
} CoeffCorrectorOpen;

typedef struct CoeffCorrectorShort
{
	ZmShort Zm[CORRECTOR_OPEN_SHORT_GAIN_COUNT];
	float R100;//real load value 100 Om
	float R1;//real load value 1 Om (for gain=7)
} CoeffCorrectorShort;

//sizeof(CoeffCorrector)<512
typedef struct CoeffCorrector
{
	uint32_t period;//period==0 - not filled
	CoeffCorrector2x x2x[CORRECTOR2X_RESISTOR_COUNT];
	CoeffCorrectorOpen open;
	CoeffCorrectorShort cshort;
} CoeffCorrector;

void CorrectorInit();

void SetCorrector2x(uint8_t resistor, uint8_t gain, float* data);
void SetCorrector2xR(uint8_t resistor, float* data);
void SetCorrectorOpen(uint8_t gain, float* data);
void SetCorrectorOpenR(uint8_t maxGainIndex, float* data);

void SetCorrectorShort(uint8_t gain, float* data);
void SetCorrectorShortR(float* data);
void SetCorrectorPeriod(uint32_t period);

/*
Коэффициэнт, на который нужно умножить R для того, чтобы избавится от 
неточности усиления при разных коэффициэнтах усиления.
*/
complexf GainCorrector(uint8_t gain_index_V, uint8_t gain_index_I);

complexf Corrector(complexf Zxm);

//Очистить весь flash необходимый для записи калибровоячных констант
bool CorrectorFlashClear();

//Записать текущие константы в нужный кусок flash.
bool CorrectorFlashCurrentData();

#endif//_CORRECTOR_H_