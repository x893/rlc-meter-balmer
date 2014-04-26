#ifndef _CORRECTOR_H_
#define _CORRECTOR_H_

#include "calc_rc.h"

#define GAIN_CORRECTOR_VALUES_COUNT 7
#define CORRECTOR2X_DIAPAZONS 3
#define PREDEFINED_PERIODS_COUNT 4

typedef struct CoeffCorrectroeGain
{
	complexf ValuesV[GAIN_CORRECTOR_VALUES_COUNT];
	complexf ValuesI[GAIN_CORRECTOR_VALUES_COUNT];
} CoeffCorrectroeGain;

typedef struct CoeffCorrector2x
{
	complexf Zm1;//measured minimal value
	complexf Zm2;//measured mmaximal value
	float Z1;//real minimal value
	float Z2;//real maximal value
} CoeffCorrector2x;

typedef struct CoeffCorrectorOpen
{
	complexf Zstdm;//measured load 100 КОм
	complexf Zom;//measured open fixtures
	float R;//precize 100 КОм real value
	float C;//capacitance load
} CoeffCorrectorOpen;

typedef struct CoeffCorrectorShort
{
	complexf Zstdm;//measured load
	complexf Zsm;//measured short
	float R;//real load value
} CoeffCorrectorShort;

//sizeof(CoeffCorrector)==256
typedef struct CoeffCorrector
{
	uint32_t period;//period==0 - not filled
	uint32_t pad;
	CoeffCorrectroeGain gain;
	CoeffCorrector2x x2x[CORRECTOR2X_DIAPAZONS];
	CoeffCorrectorOpen open;
	CoeffCorrectorShort short100;
	CoeffCorrectorShort short1;
} CoeffCorrector;

void CorrectorInit();

void SetGainCorrectorV(float* data);
void SetGainCorrectorI(float* data);
void SetCorrector2x(uint8_t diapazon, float* data);
void SetCorrectorOpen(float* data);
void SetCorrectorShort(bool is1Om, float* data);
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