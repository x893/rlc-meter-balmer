#ifndef _CALC_RC_H_
#define _CALC_RC_H_

#include <complex.h>
typedef complex float complexf;


#include "adc.h"
#include "process_measure.h"

void OnCalculate();

extern complexf lastZx;
extern bool lastZxFilled;


#endif//_CALC_RC_H_