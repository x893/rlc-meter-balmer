#ifndef _CORRECTOR_H_
#define _CORRECTOR_H_

#include "calc_rc.h"

#define GAIN_CORRECTOR_VALUES_COUNT 7

void SetGAinCorrectorV(float* data);
void SetGAinCorrectorI(float* data);

/*
Коэффициэнт, на который нужно умножить R для того, чтобы избавится от 
неточности усиления при разных коэффициэнтах усиления.
*/
complexf GainCorrector(uint8_t gain_index_V, uint8_t gain_index_I);


#endif//_CORRECTOR_H_