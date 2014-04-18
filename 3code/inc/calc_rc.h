#ifndef _CALC_RC_H_
#define _CALC_RC_H_

#include "adc.h"

typedef enum STATES
{
	STATE_NOP=0,
	STATE_START_GAIN_AUTO,
	STATE_RESISTOR_INDEX,
	STATE_RESISTOR_INDEX_WAIT,
	STATE_GAIN_INDEX,
	STATE_GAIN_INDEX_WAIT,
} STATES;

void ProcessData();
void ProcessSetState(STATES state);
STATES ProcessGetState();

extern uint8_t resistorIdx;
extern uint8_t gainVoltageIdx;
extern uint8_t gainCurrentIdx;

#endif//_CALC_RC_H_