#include "hw_config.h"
#include <complex.h>

#include "corrector.h"


complexf gainCorrectorValuesV[GAIN_CORRECTOR_VALUES_COUNT] = {1,1,1,1,1,1,1};
complexf gainCorrectorValuesI[GAIN_CORRECTOR_VALUES_COUNT] = {1,1,1,1,1,1,1};

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