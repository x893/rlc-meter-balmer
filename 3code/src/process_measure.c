// balmer@inbox.ru RLC Meter 303
// 2013-2014

#include "hw_config.h"
#include "usb_commands.h"
#include "calc_rc.h"
#include "mcp6s21.h"
#include "lcd_interface.h"
#include "dac.h"
#include "corrector.h"
#include "menu.h"

#define goodMin 2700
#define goodMax 3700
#define goodDelta (goodMax-goodMin)

const uint8_t gainValues[] = { 1, 2, 4, 5, 8, 10, 16, 32 };
const uint8_t gainIdxPtrCentral[] = { 0, 1, 2 };
const uint8_t gainIdxPtrOpenShort[] = { 0, 1, 2, 4, 6, 7 };

extern int printD;

AdcSummaryData sum_data;
Measure_Context_t Measure_Context;

void OnStartGainAuto	(void);
void OnResistorIndex	(void);
void OnStartGainIndex	(void);
void OnGainIndex		(void);
void OnMeasure			(void);
void OnMeasureStart		(void);
void OnInitWait			(void);

void SetGainCentralIdx()
{
	Measure_Context.gainIdxPtr = gainIdxPtrCentral;
	Measure_Context.gainIdxCount = NELEMENTS(gainIdxPtrCentral);
}

void SetGainOpenShortIdx()
{
	Measure_Context.gainIdxPtr = gainIdxPtrOpenShort;
	Measure_Context.gainIdxCount = NELEMENTS(gainIdxPtrOpenShort);
}

float getGainValue(uint8_t idx)
{
	float mulX = 1.0f;
	if (idx < NELEMENTS(gainValues))
		mulX = gainValues[idx];
	return (3.74f * mulX);
}

float getGainValueV()
{
	return getGainValue(Measure_Context.gainVoltageIdx);
}

float getGainValueI()
{
	return getGainValue(Measure_Context.gainCurrentIdx);
}

float getResistorOm()
{
	float R = 100.0;
	switch (Measure_Context.resistorIdx)
	{
	case 0: R = 1e2f; break;
	case 1: R = 1e3f; break;
	case 2: R = 1e4f; break;
	case 3: R = 1e5f; break;
	default:;
	}
	return R;
}

uint16_t ProcessCalcOptimalCount()
{
	uint16_t count = 1;
	int F = (int)DacFrequency();

	if (F < 2000)
		count = F / 10;
	else
		count = F / 30;

	if (count > 200)
		count = 200;
	else if (count < 1)
		count = 1;

	return count;
}

void ProcessStartComputeX(
	uint16_t count,
	uint8_t resistorIdx,
	uint8_t gainVoltageIdx,
	uint8_t gainCurrentIdx,
	bool useCorrector
	)
{
	SetGainCentralIdx();
	Measure_Context.computeXCount = count;
	if (count == 0)
		Measure_Context.computeXCount = ProcessCalcOptimalCount();

	Measure_Context.predefinedResistorIdx = resistorIdx;
	Measure_Context.predefinedGainVoltageIdx = gainVoltageIdx;
	Measure_Context.predefinedGainCurrentIdx = gainCurrentIdx;
	Measure_Context.useCorrector = useCorrector;

	OnStartGainAuto();
	AdcUsbRequestData();
}

STATES ProcessGetState()
{
	return Measure_Context.state;
}

void ProcessData()
{
	switch (Measure_Context.state)
	{
	case STATE_NOP:
		return;
	case STATE_INIT_WAIT:
		OnInitWait();
		return;
	case STATE_RESISTOR_INDEX:
		OnResistorIndex();
		break;
	case STATE_RESISTOR_INDEX_WAIT:
		Measure_Context.state = STATE_RESISTOR_INDEX;
		break;
	case STATE_GAIN_INDEX:
		OnGainIndex();
		break;
	case STATE_GAIN_INDEX_WAIT:
		Measure_Context.state = STATE_GAIN_INDEX;
		break;
	case STATE_MEASURE:
		OnMeasure();
		break;
	case STATE_MEASURE_WAIT:
		Measure_Context.state = STATE_MEASURE;
		break;
	}
}

void OnStartGainAuto()
{
	if (!Measure_Context.debugRepaint)
		LcdRepaint();

	CorrectorLoadData();

	Measure_Context.resistorIdx = 0;
	Measure_Context.gainVoltageIdx = 0;
	Measure_Context.gainCurrentIdx = 0;

	if (Measure_Context.predefinedGainVoltageIdx != 255 && Measure_Context.predefinedGainCurrentIdx != 255)
	{
		Measure_Context.gainVoltageIdx = Measure_Context.predefinedGainVoltageIdx;
		Measure_Context.gainCurrentIdx = Measure_Context.predefinedGainCurrentIdx;
	}

	MCPSetGain(true, Measure_Context.gainVoltageIdx);
	MCPSetGain(false, Measure_Context.gainCurrentIdx);

	SetLowPassFilter(DacPeriod() >= LOW_PASS_PERIOD);

	if (Measure_Context.predefinedResistorIdx != 255)
	{
		Measure_Context.resistorIdx = Measure_Context.predefinedResistorIdx;
		SetResistor(Measure_Context.resistorIdx);
	}
	else
	{
		SetResistor(Measure_Context.resistorIdx);
	}

	Measure_Context.initWaitCount = Measure_Context.bCalibration ? 10 : 2;
	Measure_Context.state = STATE_INIT_WAIT;
}

void OnInitWait()
{
	if (Measure_Context.initWaitCount > 0)
	{
		Measure_Context.initWaitCount--;
		return;
	}

	if (Measure_Context.predefinedGainVoltageIdx != 255 && Measure_Context.predefinedGainCurrentIdx != 255)
		OnMeasureStart();
	else if (Measure_Context.predefinedResistorIdx != 255)
		OnStartGainIndex();
	else
		Measure_Context.state = STATE_RESISTOR_INDEX_WAIT;
}

void OnResistorIndex()
{
	AdcSummaryChannel* asc = &g_data.ch_i;
	int di = asc->adc_max - asc->adc_min;
	if (di * 10 > goodDelta || Measure_Context.resistorIdx >= 3)
	{
		OnStartGainIndex();
	}
	else
	{
		Measure_Context.state = STATE_RESISTOR_INDEX_WAIT;
		Measure_Context.resistorIdx++;
		SetResistor(Measure_Context.resistorIdx);
	}
}

void OnStartGainIndex()
{
	Measure_Context.state = STATE_GAIN_INDEX;

	Measure_Context.gainIndexStopV = false;
	Measure_Context.gainIndexStopI = false;
	Measure_Context.gainIndexIterator = 0;
	Measure_Context.gainVoltageIdx = 0;
	Measure_Context.gainCurrentIdx = 0;

	if (Measure_Context.resistorIdx == 0)
	{
		int vmin = g_data.ch_v.adc_min;
		int vmax = g_data.ch_v.adc_max;
		int imin = g_data.ch_i.adc_min;
		int imax = g_data.ch_i.adc_max;
		if ((imax - imin) < (vmax - vmin))
		{
			Measure_Context.gainIndexStopV = true;
			SetGainCentralIdx();
		}
		else
		{
			Measure_Context.gainIndexStopI = true;
			SetGainOpenShortIdx();
		}
	}
	else if (Measure_Context.resistorIdx == 3)
	{
		Measure_Context.gainIndexStopV = true;
		SetGainOpenShortIdx();
	}
	else
	{
		Measure_Context.gainIndexStopV = true;
		SetGainCentralIdx();
	}

	OnGainIndex();
}

void OnGainIndex()
{
	int vmin = g_data.ch_v.adc_min;
	int vmax = g_data.ch_v.adc_max;
	int imin = g_data.ch_i.adc_min;
	int imax = g_data.ch_i.adc_max;

	uint8_t gainIdx = Measure_Context.gainIdxPtr[Measure_Context.gainIndexIterator];

	CoeffCorrector* corr = GetCorrector();
	if (Measure_Context.resistorIdx == 3 && gainIdx > corr->open.maxGainIndex)
	{
		Measure_Context.gainIndexStopI = true;
		if (Measure_Context.debugRepaint)
			printD = 135;
	}
	else
	{
		if (Measure_Context.debugRepaint)
			printD = 246;
	}

	if (!Measure_Context.gainIndexStopV && vmax<goodMax && vmin>goodMin)
	{
		Measure_Context.gainVoltageIdx = gainIdx;
	}
	else
	{
		MCPSetGain(true, Measure_Context.gainVoltageIdx);
		Measure_Context.gainIndexStopV = true;
	}

	if (!Measure_Context.gainIndexStopI && imax<goodMax && imin>goodMin)
	{
		Measure_Context.gainCurrentIdx = gainIdx;
	}
	else
	{
		MCPSetGain(false, Measure_Context.gainCurrentIdx);
		Measure_Context.gainIndexStopI = true;
	}

	Measure_Context.gainIndexIterator++;
	if (Measure_Context.gainIndexIterator >= Measure_Context.gainIdxCount)
	{
		MCPSetGain(true, Measure_Context.gainVoltageIdx);
		MCPSetGain(false, Measure_Context.gainCurrentIdx);
		OnMeasureStart();//state = STATE_NOP;
	}
	else
	{
		gainIdx = Measure_Context.gainIdxPtr[Measure_Context.gainIndexIterator];
		if (!Measure_Context.gainIndexStopV)
			MCPSetGain(true, gainIdx);
		if (!Measure_Context.gainIndexStopI)
			MCPSetGain(false, gainIdx);
		Measure_Context.state = STATE_GAIN_INDEX_WAIT;
	}

	if (Measure_Context.debugRepaint)
		LcdRepaint();
}

void OnMeasureStart()
{
	LcdRepaint();
	Measure_Context.state = STATE_MEASURE_WAIT;
	Measure_Context.computeXIterator = 0;
}

void OnMeasure()
{
	bool oldLastZxFilled;

	if (Measure_Context.computeXIterator == 0)
		sum_data = g_data;
	else
	{
		sum_data.ch_i.k_sin += g_data.ch_i.k_sin;
		sum_data.ch_i.k_cos += g_data.ch_i.k_cos;
		sum_data.ch_i.square_error += g_data.ch_i.square_error;

		sum_data.ch_v.k_sin += g_data.ch_v.k_sin;
		sum_data.ch_v.k_cos += g_data.ch_v.k_cos;
		sum_data.ch_v.square_error += g_data.ch_v.square_error;

		if (g_data.error)
			sum_data.error = true;
	}

	Measure_Context.computeXIterator++;
	if (Measure_Context.computeXIterator < Measure_Context.computeXCount)
		return;

	// calculate result
	sum_data.ch_i.k_sin /= Measure_Context.computeXCount;
	sum_data.ch_i.k_cos /= Measure_Context.computeXCount;
	sum_data.ch_i.square_error /= Measure_Context.computeXCount;

	sum_data.ch_v.k_sin /= Measure_Context.computeXCount;
	sum_data.ch_v.k_cos /= Measure_Context.computeXCount;
	sum_data.ch_v.square_error /= Measure_Context.computeXCount;

	g_data = sum_data;

	oldLastZxFilled = lastZxFilled;
	complexf oldLastZx = lastZx;

	OnCalculate(Measure_Context.useCorrector);

	LcdRepaint();

	if (Measure_Context.bCalibration)
	{
		Measure_Context.state = STATE_NOP;
		OnCalibrationComplete();
	}
	else if (Measure_Context.bContinuousMode)
	{
		bool startFast = false;
		if (oldLastZxFilled)
		{
			float d = cabs(oldLastZx - lastZx) / cabs(lastZx);
			if (d > 1)
				d = 1;
			if (d < 5e-2f)
			{
				int vmin = g_data.ch_v.adc_min;
				int vmax = g_data.ch_v.adc_max;
				int imin = g_data.ch_i.adc_min;
				int imax = g_data.ch_i.adc_max;

				if (vmax<goodMax && vmin>goodMin && imax<goodMax && imin>goodMin)
				{
					//Все хорошо, мтожно не пересчитывать коэффициэнты и не переставлять резистор
					startFast = true;
				}
			}
		}

		if (startFast)
			OnMeasureStart();
		else
			ProcessStartComputeX(
				Measure_Context.computeXCount,
				Measure_Context.predefinedResistorIdx,
				Measure_Context.predefinedGainVoltageIdx,
				Measure_Context.predefinedGainCurrentIdx,
				Measure_Context.useCorrector
				);
	}
	else
		Measure_Context.state = STATE_NOP;
}

void SendRVI()
{
	USBAdd8(Measure_Context.resistorIdx);
	USBAdd8(Measure_Context.gainVoltageIdx);
	USBAdd8(Measure_Context.gainCurrentIdx);
}
