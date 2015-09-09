// balmer@inbox.ru RLC Meter 303
// 2013-2014

#include "hw_config.h"
#include <complex.h>
#include <stm32f30x_flash.h>

#include "main.h"
#include "corrector.h"
#include "dac.h"
/*
period= 720000	freq=     100
period= 72000	freq=    1000
period= 7200	freq=  10 000
period= 768		freq=  93 750
period= 384		freq= 187 500
*/

const uint32_t predefinedPeriods[] = { 720000, 72000, 7200, 768, 384 };
const int8_t gainValidIdx[] = { 0, 1, 2, -1, 3, -1, 4, 5 };

complexf Corrector2x(complexf Zxm, CoeffCorrector2x* c);
complexf CorrectorOpen(complexf Zxm, CoeffCorrectorOpen* c);
complexf CorrectorShort(complexf Zxm, CoeffCorrectorShort* c);
void CorrectorLoadData(void);

static CoeffCorrector coeff;

CoeffCorrector* GetCorrector()
{
	return &coeff;
}

void CorrectorInit()
{
	coeff.period = 0;
	coeff.open.maxGainIndex = 7;
}

void SetCorrectorPeriod(uint32_t period)
{
	coeff.period = period;
}

int8_t GetGainValidIdx(uint8_t index)
{
	return gainValidIdx[index];
}

complexf Corrector(complexf Zxm)
{
	CorrectorLoadData();

	if (coeff.period == 0)
		return Zxm;

	bool is_short = false;
	if (Measure_Context.gainVoltageIdx > 0)
		is_short = true;
	else if (Measure_Context.gainCurrentIdx > 0)
		is_short = false;
	else
		is_short = cabs(Zxm) < 100;

	if (is_short)
		return CorrectorShort(Zxm, &coeff.cshort);

	if (Measure_Context.resistorIdx < CORRECTOR2X_RESISTOR_COUNT)
		return Corrector2x(Zxm, coeff.x2x + Measure_Context.resistorIdx);

	if (Measure_Context.resistorIdx == 3)
		return CorrectorOpen(Zxm, &coeff.open);

	return Zxm;
}

void SetCorrector2x(uint8_t resistor, uint8_t gain, float* data)
{
	if (resistor >= CORRECTOR2X_RESISTOR_COUNT || gain >= CORRECTOR2X_GAIN_COUNT)
		return;

	CoeffCorrector2x* cr = coeff.x2x + resistor;
	ZmOpen* c = cr->Zm + gain;
	c->Zstdm = data[0] + data[1] * I;
	c->Zom = data[2] + data[3] * I;
	cr->R[gain] = data[4];
	cr->C[gain] = data[5];
}

void SetCorrector2xR(uint8_t resistor, float* data)
{
	if (resistor >= CORRECTOR2X_RESISTOR_COUNT)
		return;
}

void SetCorrectorOpen(uint8_t gain, float* data)
{
	if (gain >= CORRECTOR_OPEN_SHORT_GAIN_COUNT)
		return;
	ZmOpen* c = coeff.open.Zm + gain;
	c->Zstdm = data[0] + data[1] * I;
	c->Zom = data[2] + data[3] * I;
}

void SetCorrectorOpenR(uint8_t maxGainIndex, float* data)
{
	CoeffCorrectorOpen* c = &coeff.open;
	c->R = data[0];
	c->C = data[1];
	c->maxGainIndex = maxGainIndex;
}

void SetCorrectorShort(uint8_t gain, float* data)
{
	if (gain >= CORRECTOR_OPEN_SHORT_GAIN_COUNT)
		return;
	ZmShort* c = coeff.cshort.Zm + gain;
	c->Zsm = data[0] + data[1] * I;
	c->Zstdm = data[2] + data[3] * I;
}

void SetCorrectorShortR(float* data)
{
	CoeffCorrectorShort* c = &coeff.cshort;
	c->R100 = data[0];
	c->R1 = data[1];
}

complexf CorrectorOpenX(complexf Zxm, float Rstd, float Cstd, complexf Zstdm, complexf Zom)
{
	float F = DacFrequency();
	complexf Ystd = 1.0f / Rstd + 2.0f * pi * F * Cstd * I;
	complexf Zstd = 1.0f / Ystd;
	complexf Zx = Zstd*(1 / Zstdm - 1 / Zom) * Zxm / (1 - Zxm / Zom);
	return Zx;
}

complexf Corrector2x(complexf Zxm, CoeffCorrector2x* cr)
{
	if (Measure_Context.gainCurrentIdx >= CORRECTOR2X_GAIN_COUNT)
		return 0;
	ZmOpen* c = cr->Zm + Measure_Context.gainCurrentIdx;
	float Rstd = cr->R[Measure_Context.gainCurrentIdx];
	float Cstd = cr->C[Measure_Context.gainCurrentIdx];
	return CorrectorOpenX(Zxm, Rstd, Cstd, c->Zstdm, c->Zom);
}

complexf CorrectorOpen(complexf Zxm, CoeffCorrectorOpen* cr)
{
	int idx = gainValidIdx[Measure_Context.gainCurrentIdx];
	if (idx < 0)
		return 0;
	ZmOpen* c = cr->Zm + idx;

	return CorrectorOpenX(Zxm, cr->R, cr->C, c->Zstdm, c->Zom);
}

complexf CorrectorShort(complexf Zxm, CoeffCorrectorShort* cr)
{
	int idx = gainValidIdx[Measure_Context.gainVoltageIdx];
	if (idx < 0)
		return 0;
	ZmShort* c = cr->Zm + idx;
	complexf Zstd = Measure_Context.gainVoltageIdx == 7 ? cr->R1 : cr->R100;
	complexf Zx = Zstd / (c->Zstdm - c->Zsm) * (Zxm - c->Zsm);
	return Zx;
}

uint32_t round256(uint32_t c)
{
	return ((c + 255) / 256) * 256;
}

//Return 255 if not found
uint8_t PredefinedPeriodIndex()
{
	uint8_t index = 255;
	uint8_t i;
	for (i = 0; i < NELEMENTS(predefinedPeriods); i++)
	{
		if (predefinedPeriods[i] == coeff.period)
		{
			index = i;
			break;
		}
	}
	return index;
}

bool CorrectorFlashClear()
{
	bool ok;
	int i;

	//Clear 2 Kb flash 
	FLASH_Unlock();
	for (i = 0; i < NELEMENTS(predefinedPeriods); i++)
	{
		ok = (FLASH_ErasePage(FLASH_START_ARRAY + COEFF_CORRECTOR_SIZE * i) == FLASH_COMPLETE);
		if (!ok)
			break;
	}
	FLASH_Lock();
	return ok;
}

bool CorrectorFlashClearCurrent()
{
	bool ok;
	uint8_t index = PredefinedPeriodIndex();

	if (index == 255)
		return false;

	//Clear 4 Kb flash 
	FLASH_Unlock();
	ok = (FLASH_ErasePage(FLASH_START_ARRAY + COEFF_CORRECTOR_SIZE * index) == FLASH_COMPLETE);
	FLASH_Lock();
	return ok;
}

bool CorrectorFlashCurrentData()
{
	uint8_t index = PredefinedPeriodIndex();

	if (index == 255)
		return false;

	uint32_t offset = index * COEFF_CORRECTOR_SIZE;

	FLASH_Unlock();

	for (int i = 0; i < sizeof(CoeffCorrector); i += 4)
	{
		FLASH_ProgramWord(FLASH_START_ARRAY + offset + i, ((uint32_t*)&coeff)[i / 4]);
	}

	FLASH_Lock();
	return true;
}

void CorrectorLoadData()
{
	CoeffCorrector* cfound = NULL;
	if (coeff.period == DacPeriod())
		return;
	for (int i = 0; i < NELEMENTS(predefinedPeriods); i++)
	{
		CoeffCorrector* c = (CoeffCorrector*)(i * COEFF_CORRECTOR_SIZE + FLASH_START_ARRAY);
		if (DacPeriod() == c->period)
		{
			cfound = c;
			break;
		}
	}

	if (cfound == NULL)
	{
		ClearCorrector();
		return;
	}

	coeff = *cfound;
}

void ClearCorrector()
{
	int idx;
	float C = 0.08e-12;

	coeff.period = 0;
	coeff.open.maxGainIndex = 7;

	coeff.cshort.R100 = 1e2;
	coeff.cshort.R1 = 1.0;
	for (idx = 0; idx < CORRECTOR_OPEN_SHORT_GAIN_COUNT; idx++)
	{
		ZmShort * Zm = coeff.cshort.Zm + idx;
		if (idx == 5)
			Zm->Zstdm = coeff.cshort.R1;
		else
			Zm->Zstdm = coeff.cshort.R100;

		Zm->Zsm = 0;
	}

	for (idx = 0; idx < CORRECTOR2X_RESISTOR_COUNT; idx++)
	{
		float R0 = 1;
		float R1 = 1;
		CoeffCorrector2x * p = coeff.x2x + idx;
		if (idx == 0)
		{
			R0 = 1e2;
			R1 = 1e3;
		}
		else if (idx == 1)
		{
			R0 = 1e3;
			R1 = 1e4;
		}
		else
		{
			R0 = 1e4;
			R1 = 1e5;
		}

		for (int igain = 0; igain < CORRECTOR2X_GAIN_COUNT; igain++)
		{
			ZmOpen * Zm = p->Zm + igain;
			p->R[igain] = igain < 2 ? R0 : R1;
			p->C[igain] = C;
			Zm->Zstdm = p->R[igain];
			Zm->Zom = 1e9;
		}
	}

	coeff.open.R = 1e5;
	coeff.open.C = C;
	coeff.open.maxGainIndex = 7;
	for (idx = 0; idx < CORRECTOR_OPEN_SHORT_GAIN_COUNT; idx++)
	{
		ZmOpen * Zm = coeff.open.Zm + idx;
		Zm->Zstdm = coeff.open.R;
		Zm->Zom = 1e9;
	}
}
