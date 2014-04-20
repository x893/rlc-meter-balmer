//
//  complex.cpp
//  RLC Meter
//
//  Created by Balmer on 13.04.14.
//
//

#include "hw_config.h"
#include "voltage.h"
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

uint8_t computeXCount;
uint8_t computeXIterator;
uint8_t predefinedResistorIdx;

extern int printDelta;

static AdcSummaryData sum_data;

void OnStartGainAuto();
void OnResistorIndex();
void OnStartGainIndex(bool wait);
void OnGainIndex();
void OnMeasure();
void OnMeasureStart();

float getGainValue(uint8_t idx)
{
    float mulPre = 3.74f;
    float mulX = 1.0f;
    if(idx<gainValuesCount)
    	mulX = gainValues[idx];
    return mulPre*mulX;
}

float getGainValueV()
{
	return getGainValue(gainVoltageIdx);
}

float getGainValueI()
{
	return getGainValue(gainCurrentIdx);
}

float getResistorOm()
{
	float R = 100.0;
	switch(resistorIdx) {
	case 0: R = 100.0f; break;
	case 1: R = 1e3f; break;
	case 2: R = 1e4f; break;
	case 3: R = 1e5f; break;
	default:;
	}
	return R;
}

void ProcessStartComputeX(uint8_t count, uint8_t predefinedResistorIdx_)
{
	if(count==0)
		count = 1;
	computeXCount = count;
	predefinedResistorIdx = predefinedResistorIdx_;
	OnStartGainAuto();
	AdcUsbRequestData();
}

STATES ProcessGetState()
{
	return state;
}

void ProcessData()
{
	switch(state)
	{
	case STATE_NOP:
		return;
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
	case STATE_MEASURE:
		OnMeasure();
		break;
	case STATE_MEASURE_WAIT:
		state = STATE_MEASURE;
		break;
	}
}

void OnStartGainAuto()
{
	resistorIdx = 0;
	gainVoltageIdx = 0;
	gainCurrentIdx = 0;
	MCPSetGain(true, gainVoltageIdx);
	MCPSetGain(false, gainCurrentIdx);

	if(predefinedResistorIdx!=255)
	{
		resistorIdx = predefinedResistorIdx;
		SetResistor(resistorIdx);
		OnStartGainIndex(true);
	} else
	{
		SetResistor(resistorIdx);
		state = STATE_RESISTOR_INDEX_WAIT;
	}


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
		OnStartGainIndex(false);
	} else
	{
		state = STATE_RESISTOR_INDEX_WAIT;
		resistorIdx++;
		SetResistor(resistorIdx);
	}

	LcdRepaint();
}

void OnStartGainIndex(bool wait)
{
	if(wait)
		state = STATE_GAIN_INDEX_WAIT;
	else
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
		OnMeasureStart();//state = STATE_NOP;
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

void OnMeasureStart()
{
	state = STATE_MEASURE_WAIT;
	computeXIterator = 0;
}

void OnMeasure()
{
	if(computeXIterator==0)
	{
		sum_data = g_data;
	} else
	{
		sum_data.ch_i.k_sin += g_data.ch_i.k_sin;
		sum_data.ch_i.k_cos += g_data.ch_i.k_cos;
		sum_data.ch_i.square_error += g_data.ch_i.square_error;

		sum_data.ch_v.k_sin += g_data.ch_v.k_sin;
		sum_data.ch_v.k_cos += g_data.ch_v.k_cos;
		sum_data.ch_v.square_error += g_data.ch_v.square_error;

		if(g_data.error)
			sum_data.error = true;			
	}

	computeXIterator++;
	if(computeXIterator<computeXCount)
	{
		return;
	}

	//calculate result
	sum_data.ch_i.k_sin /= computeXCount;
	sum_data.ch_i.k_cos /= computeXCount;
	sum_data.ch_i.square_error /= computeXCount;

	sum_data.ch_v.k_sin /= computeXCount;
	sum_data.ch_v.k_cos /= computeXCount;
	sum_data.ch_v.square_error /= computeXCount;

	g_data = sum_data;

	OnCalculate();
	LcdRepaint();

	state = STATE_NOP;
}

void SendRVI()
{
	USBAdd8(resistorIdx);
	USBAdd8(gainVoltageIdx);
	USBAdd8(gainCurrentIdx);
}
