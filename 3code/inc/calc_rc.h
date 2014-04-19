#ifndef _CALC_RC_H_
#define _CALC_RC_H_

#include "adc.h"

typedef enum STATES
{
	STATE_NOP=0,
	STATE_RESISTOR_INDEX,
	STATE_RESISTOR_INDEX_WAIT,
	STATE_GAIN_INDEX,
	STATE_GAIN_INDEX_WAIT,
	STATE_COMPUTE_X,
	STATE_COMPUTE_X_WAIT,
} STATES;

void ProcessData();
void ProcessStartComputeX(uint8_t count, uint8_t predefinedResistorIdx);
STATES ProcessGetState();

extern uint8_t resistorIdx;
extern uint8_t gainVoltageIdx;
extern uint8_t gainCurrentIdx;

#endif//_CALC_RC_H_