#ifndef _CORRECTOR_H_
#define _CORRECTOR_H_

#include "calc_rc.h"

#define GAIN_CORRECTOR_VALUES_COUNT 7
#define CORRECTOR2X_DIAPAZONS 3

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
	CoeffCorrectroeGain gain;
	CoeffCorrector2x x2x[CORRECTOR2X_DIAPAZONS];
	CoeffCorrectorOpen open;
	CoeffCorrectorShort short100;
	CoeffCorrectorShort short1;
	uint32_t period;
	uint32_t pad;
} CoeffCorrector;

void CorrectorInit();

void SetGAinCorrectorV(float* data);
void SetGAinCorrectorI(float* data);
void SetCorrector2x(uint8_t diapazon, float* data);
void SetCorrectorOpen(float* data);
void SetCorrectorShort(bool is1Om, float* data);

/*
Коэффициэнт, на который нужно умножить R для того, чтобы избавится от 
неточности усиления при разных коэффициэнтах усиления.
*/
complexf GainCorrector(uint8_t gain_index_V, uint8_t gain_index_I);

complexf Corrector(complexf Zxm);


#endif//_CORRECTOR_H_