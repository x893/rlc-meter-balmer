//
//  complex.cpp
//  RLC Meter
//
//  Created by Balmer on 13.04.14.
//
//

#include "hw_config.h"
#include <complex.h>
#include <math.h>
#include <stdbool.h>
#include "calc_rc.h"
#include "mcp6s21.h"
#include "lcd_interface.h"

#define goodMin 1500
#define goodMax 4000
#define goodDelta (goodMax-goodMin)


static STATES state = STATE_NOP;

#define gainValuesCount 8
uint8_t gainValues[gainValuesCount] = {1,2,4,5,8,10,16,32};


uint8_t resistorIdx;
uint8_t gainVoltageIdx;
uint8_t gainCurrentIdx;

uint8_t gainIndexIterator;
bool gainIndexStopV;
bool gainIndexStopI;

extern int printDelta;

void OnStartGainAuto();
void OnResistorIndex();
void OnStartGainIndex();
void OnGainIndex();

void ProcessSetState(STATES state_)
{
	state = state_;
}

STATES ProcessGetState()
{
	return state;
}

/*
typedef complex float complexf;

void f()
{
    complexf p1 = 1.0+2.0fi;
    complexf p2 = 3.0+4.1fi;
    complexf pd = p1/p2;
    float pf = cabsf(p1);
}
*/
void ProcessData()
{
	switch(state)
	{
	case STATE_NOP:
		return;
	case STATE_START_GAIN_AUTO:
		OnStartGainAuto();
		break;
	case STATE_RESISTOR_INDEX:
		OnResistorIndex();
		break;
	case STATE_RESISTOR_INDEX_WAIT:
		state = STATE_RESISTOR_INDEX;
		break;
	case STATE_GAIN_INDEX:
		OnGainIndex();
		break;
	case STATE_GAIN_INDEX_WAIT:
		state = STATE_GAIN_INDEX;
		break;
	}
}

void OnStartGainAuto()
{
	resistorIdx = 0;
	gainVoltageIdx = 0;
	gainCurrentIdx = 0;
	SetResistor(resistorIdx);
	MCPSetGain(true, gainVoltageIdx);
	MCPSetGain(false, gainCurrentIdx);
	state = STATE_RESISTOR_INDEX_WAIT;

	printDelta = 123;
	LcdRepaint();
}

void OnResistorIndex()
{
	AdcSummaryChannel* pi = &g_data.ch_i;
	int di = pi->adc_max-pi->adc_min;
	printDelta = di;
	if(di*10>goodDelta || resistorIdx>=3)
	{
		OnStartGainIndex();
	} else
	{
		state = STATE_RESISTOR_INDEX_WAIT;
		resistorIdx++;
		SetResistor(resistorIdx);
	}

	LcdRepaint();
}

void OnStartGainIndex()
{
	state = STATE_GAIN_INDEX;
    gainIndexStopV = false;
    gainIndexStopI = false;
    gainIndexIterator = 0;

	OnGainIndex();
}

void OnGainIndex()
{
	int vmin = g_data.ch_v.adc_min;
	int vmax = g_data.ch_v.adc_max;
	int imin = g_data.ch_i.adc_min;
	int imax = g_data.ch_i.adc_max;
	printDelta = gainIndexIterator+100;

	if(!gainIndexStopV && vmax<goodMax && vmin>goodMin)
	{
		gainVoltageIdx = gainIndexIterator;
	} else
	{
		MCPSetGain(true, gainVoltageIdx);
		gainIndexStopV = true;
	}

	if(!gainIndexStopI && imax<goodMax && imin>goodMin)
	{
		gainCurrentIdx = gainIndexIterator;
	} else
	{
		MCPSetGain(false, gainCurrentIdx);
		gainIndexStopI = true;
	}

	gainIndexIterator++;
	if(gainIndexIterator>=gainValuesCount)
	{
		MCPSetGain(true, gainVoltageIdx);
		MCPSetGain(false, gainCurrentIdx);
		state = STATE_NOP;
	} else
	{
		if(!gainIndexStopV)
			MCPSetGain(true, gainIndexIterator);
		if(!gainIndexStopI)
			MCPSetGain(false, gainIndexIterator);
		state = STATE_GAIN_INDEX_WAIT;
	}

	LcdRepaint();
}
