#include "hw_config.h"
#include <complex.h>

#include "corrector.h"


complexf gainCorrectorValuesV[GAIN_CORRECTOR_VALUES_COUNT] = {1,1,1,1,1,1,1};
complexf gainCorrectorValuesI[GAIN_CORRECTOR_VALUES_COUNT] = {1,1,1,1,1,1,1};

CoeffCorrector2x coeffCorrector2x[CORRECTOR2X_DIAPAZONS];

void CorrectorInit()
{
	int i;
	for(i=0; i<GAIN_CORRECTOR_VALUES_COUNT; i++)
	{
		gainCorrectorValuesV[i] = 1;
		gainCorrectorValuesI[i] = 1;
	}

	coeffCorrector2x[0].Zm1 = 1e2;
	coeffCorrector2x[0].Zm2 = 1e3;
	coeffCorrector2x[0].Z1 = 1e2;
	coeffCorrector2x[0].Z2 = 1e3;

	coeffCorrector2x[1].Zm1 = 1e3;
	coeffCorrector2x[1].Zm2 = 1e4;
	coeffCorrector2x[1].Z1 = 1e3;
	coeffCorrector2x[1].Z2 = 1e4;

	coeffCorrector2x[2].Zm1 = 1e4;
	coeffCorrector2x[2].Zm2 = 1e5;
	coeffCorrector2x[2].Z1 = 1e4;
	coeffCorrector2x[2].Z2 = 1e5;
}


void SetGAinCorrectorV(float* data)
{
	for(int i=0; i<GAIN_CORRECTOR_VALUES_COUNT; i++)
	{
		gainCorrectorValuesV[i] = data[i*2]+data[i*2+1]*I;
	}
}

void SetGAinCorrectorI(float* data)
{
	for(int i=0; i<GAIN_CORRECTOR_VALUES_COUNT; i++)
	{
		gainCorrectorValuesI[i] = data[i*2]+data[i*2+1]*I;
	}
}


complexf GainCorrector(uint8_t gain_index_V, uint8_t gain_index_I)
{
	if(gain_index_V>=GAIN_CORRECTOR_VALUES_COUNT)
		gain_index_V = GAIN_CORRECTOR_VALUES_COUNT-1;
	if(gain_index_I>=GAIN_CORRECTOR_VALUES_COUNT)
		gain_index_I = GAIN_CORRECTOR_VALUES_COUNT-1;

	complexf pV = gainCorrectorValuesV[0]/gainCorrectorValuesV[gain_index_V];
	complexf pI = gainCorrectorValuesI[0]/gainCorrectorValuesI[gain_index_I];

	return pV/pI;
}

complexf Corrector(complexf Zxm)
{
	if(resistorIdx<CORRECTOR2X_DIAPAZONS)
	{
		return Corrector2x(Zxm, coeffCorrector2x+resistorIdx);
	}

	return Zxm;
}

void SetCorrector2x(uint8_t diapazon, float* data)
{
	if(diapazon>=CORRECTOR2X_DIAPAZONS)
		return;
	CoeffCorrector2x* c = coeffCorrector2x+diapazon;
	c->Zm1 = data[0]+data[1]*I;
	c->Zm2 = data[2]+data[3]*I;
	c->Z1 = data[4];
	c->Z2 = data[5];
}


complexf Corrector2x(complexf Zxm, CoeffCorrector2x* c)
{
	complexf A = (c->Z2-c->Z1)/(c->Zm2-c->Zm1);
	complexf B = (c->Z1*c->Zm2-c->Z2*c->Zm1)/(c->Zm2-c->Zm1);
	complexf Zx = A*Zxm+B;
	return Zx;
}
