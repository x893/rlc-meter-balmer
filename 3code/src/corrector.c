// balmer@inbox.ru RLC Meter 303
// 2013-2014

#include "hw_config.h"
#include <complex.h>
#include <stm32f30x_flash.h>

#include "main.h"
#include "corrector.h"
#include "dac.h"
/*
period= 720000 freq= 100
period= 72000 freq= 1000
period= 7200 freq= 10000
period= 768 freq=  93750 
*/
const uint32_t predefinedPeriods[PREDEFINED_PERIODS_COUNT] = {720000, 72000, 7200, 768, 288};
const signed char gainValidIdx[] = {0, 1, 2, -1, 3, -1, 4, 5};

complexf Corrector2x(complexf Zxm, CoeffCorrector2x* c);
complexf CorrectorOpen(complexf Zxm, CoeffCorrectorOpen* c);
complexf CorrectorShort(complexf Zxm, CoeffCorrectorShort* c);
void CorrectorLoadData();

static CoeffCorrector coeff;
extern int printD;

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


complexf Corrector(complexf Zxm)
{
	CorrectorLoadData();

	if(coeff.period==0)
		return Zxm;

	if(cabs(Zxm)<100)
	{
		return CorrectorShort(Zxm, &coeff.cshort);	
	}

	if(resistorIdx<CORRECTOR2X_RESISTOR_COUNT)
	{
		return Corrector2x(Zxm, coeff.x2x+resistorIdx);
	}

	if(resistorIdx==3)
	{
		return CorrectorOpen(Zxm, &coeff.open);
	}

	return Zxm;
}

void SetCorrector2x(uint8_t resistor, uint8_t gain, float* data)
{
	if(resistor>=CORRECTOR2X_RESISTOR_COUNT || gain>=CORRECTOR2X_GAIN_COUNT)
		return;

	Zm2x* c = coeff.x2x[resistor].Zm+gain;
	c->Zm1 = data[0]+data[1]*I;
	c->Zm2 = data[2]+data[3]*I;
}

void SetCorrector2xR(uint8_t resistor, float* data)
{
	if(resistor>=CORRECTOR2X_RESISTOR_COUNT)
		return;

	coeff.x2x[resistor].Z1 = data[0];
	coeff.x2x[resistor].Z2 = data[1];
}

void SetCorrectorOpen(uint8_t gain, float* data)
{
	if(gain>=CORRECTOR_OPEN_SHORT_GAIN_COUNT)
		return;
	ZmOpen* c = coeff.open.Zm+gain;
	c->Zstdm = data[0]+data[1]*I;
	c->Zom = data[2]+data[3]*I;
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
	if(gain>=CORRECTOR_OPEN_SHORT_GAIN_COUNT)
		return;
	ZmShort* c = coeff.cshort.Zm+gain;
	c->Zsm = data[0]+data[1]*I;
	c->Zstdm = data[2]+data[3]*I;
}

void SetCorrectorShortR(float* data)
{
	CoeffCorrectorShort* c = &coeff.cshort;
	c->R100 = data[0];
	c->R1 = data[1];
}


complexf Corrector2x(complexf Zxm, CoeffCorrector2x* cr)
{
	Zm2x* c = NULL;
	if(gainCurrentIdx>=CORRECTOR2X_GAIN_COUNT)
		return 0;
	c = cr->Zm+gainCurrentIdx;	
	complexf A = (cr->Z2-cr->Z1)/(c->Zm2-c->Zm1);
	complexf B = (cr->Z1*c->Zm2-cr->Z2*c->Zm1)/(c->Zm2-c->Zm1);
	complexf Zx = A*Zxm+B;
	return Zx;
}

complexf CorrectorOpen(complexf Zxm, CoeffCorrectorOpen* cr)
{
	float F = DacFrequency();
	int idx = gainValidIdx[gainCurrentIdx];
	if(idx<0)
		return 0;
	ZmOpen* c = cr->Zm+idx;

	complexf Ystd = 1.0f/cr->R + 2.0f*pi*F*cr->C*I;
	complexf Zstd = 1.0f/Ystd;
	complexf Zx = Zstd*(1/c->Zstdm-1/c->Zom)*Zxm/(1-Zxm/c->Zom);
	return Zx;
}

complexf CorrectorShort(complexf Zxm, CoeffCorrectorShort* cr)
{
	int idx = gainValidIdx[gainVoltageIdx];
	if(idx<0)
		return 0;
	ZmShort* c = cr->Zm+idx;
	complexf Zstd = gainVoltageIdx==7 ? cr->R1 : cr->R100;
	complexf Zx = Zstd/(c->Zstdm-c->Zsm)*(Zxm-c->Zsm);
	return Zx;	
}

bool CorrectorFlashClear()
{
	//Clear 2 Kb flash 
	bool ok;
	FLASH_Unlock();
	ok = (FLASH_ErasePage(FLASH_START_ARRAY)==FLASH_COMPLETE);
	FLASH_Lock();
	return ok;
}

uint32_t round256(uint32_t c)
{
	return ((c+255)/256)*256;
}

bool CorrectorFlashCurrentData()
{
	bool found = false;
	uint32_t index;
	for(int i=0; i<PREDEFINED_PERIODS_COUNT; i++)
	{
		if(predefinedPeriods[i]==coeff.period)
		{
			found = true;
			index = i;
			break;
		}
	}

	if(!found)
		return false;

	uint32_t offset = index*round256(sizeof(CoeffCorrector));

	FLASH_Unlock();

	for(int i=0; i<sizeof(CoeffCorrector); i+=4)
	{
		FLASH_ProgramWord(i+offset+FLASH_START_ARRAY, ((uint32_t*)&coeff)[i/4]);
	}

	FLASH_Lock();
	return true;
}

void CorrectorLoadData()
{
	CoeffCorrector* cfound = NULL;
	if(coeff.period==DacPeriod())
		return;
	for(int i=0; i<PREDEFINED_PERIODS_COUNT; i++)
	{
		CoeffCorrector* c = (CoeffCorrector*)(i*round256(sizeof(CoeffCorrector))+FLASH_START_ARRAY);
		if(DacPeriod()==c->period)
		{
			cfound = c;
			break;
		}
	}

	if(cfound==NULL)
	{
		CorrectorInit();
		return;
	}

	coeff = *cfound;
}
