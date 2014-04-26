#include "hw_config.h"
#include <complex.h>

#include "corrector.h"
#include "dac.h"

complexf Corrector2x(complexf Zxm, CoeffCorrector2x* c);
complexf CorrectorOpen(complexf Zxm, CoeffCorrectorOpen* c);
complexf CorrectorShort(complexf Zxm, CoeffCorrectorShort* c);

static CoeffCorrector coeff;

uint32_t corrSize =	sizeof(CoeffCorrector);


void CorrectorInit()
{
}

void SetGAinCorrectorV(float* data)
{
	for(int i=0; i<GAIN_CORRECTOR_VALUES_COUNT; i++)
	{
		coeff.gain.ValuesV[i] = data[i*2]+data[i*2+1]*I;
	}
}

void SetGAinCorrectorI(float* data)
{
	for(int i=0; i<GAIN_CORRECTOR_VALUES_COUNT; i++)
	{
		coeff.gain.ValuesI[i] = data[i*2]+data[i*2+1]*I;
	}
}


complexf GainCorrector(uint8_t gain_index_V, uint8_t gain_index_I)
{
	if(gain_index_V>=GAIN_CORRECTOR_VALUES_COUNT)
		gain_index_V = GAIN_CORRECTOR_VALUES_COUNT-1;
	if(gain_index_I>=GAIN_CORRECTOR_VALUES_COUNT)
		gain_index_I = GAIN_CORRECTOR_VALUES_COUNT-1;

	complexf pV = coeff.gain.ValuesV[0]/coeff.gain.ValuesV[gain_index_V];
	complexf pI = coeff.gain.ValuesI[0]/coeff.gain.ValuesI[gain_index_I];

	return pV/pI;
}


complexf Corrector(complexf Zxm)
{
	if(gainVoltageIdx==7)
	{
		return CorrectorShort(Zxm, &coeff.short1);
	}

	Zxm *= GainCorrector(gainVoltageIdx, gainCurrentIdx);

	if(cabs(Zxm)<100)
	{
		return CorrectorShort(Zxm, &coeff.short100);	
	}

	if(resistorIdx<CORRECTOR2X_DIAPAZONS)
	{
		return Corrector2x(Zxm, coeff.x2x+resistorIdx);
	}

	if(resistorIdx==3)
	{
		return CorrectorOpen(Zxm, &coeff.open);
	}

	return Zxm;
}

void SetCorrector2x(uint8_t diapazon, float* data)
{
	if(diapazon>=CORRECTOR2X_DIAPAZONS)
		return;
	CoeffCorrector2x* c = coeff.x2x+diapazon;
	c->Zm1 = data[0]+data[1]*I;
	c->Zm2 = data[2]+data[3]*I;
	c->Z1 = data[4];
	c->Z2 = data[5];
}

void SetCorrectorOpen(float* data)
{
	CoeffCorrectorOpen* c = &coeff.open;
	c->Zstdm = data[0]+data[1]*I;
	c->Zom = data[2]+data[3]*I;
	c->R = data[4];
	c->C = data[5];
}

void SetCorrectorShort(bool is1Om, float* data)
{
	CoeffCorrectorShort* c = is1Om?&coeff.short1:&coeff.short100;
	c->Zstdm = data[0]+data[1]*I;
	c->Zsm = data[2]+data[3]*I;
	c->R = data[4];
}


complexf Corrector2x(complexf Zxm, CoeffCorrector2x* c)
{
	complexf A = (c->Z2-c->Z1)/(c->Zm2-c->Zm1);
	complexf B = (c->Z1*c->Zm2-c->Z2*c->Zm1)/(c->Zm2-c->Zm1);
	complexf Zx = A*Zxm+B;
	return Zx;
}

complexf CorrectorOpen(complexf Zxm, CoeffCorrectorOpen* c)
{
	float F = DacFrequency();
	complexf Ystd = 1.0f/c->R + 2.0f*pi*F*c->C*I;
	complexf Zstd = 1.0f/Ystd;
	complexf Zx = Zstd*(1/c->Zstdm-1/c->Zom)*Zxm/(1-Zxm/c->Zom);
	return Zx;
}

complexf CorrectorShort(complexf Zxm, CoeffCorrectorShort* c)
{
	complexf Zstd = c->R;
	complexf Zx = Zstd/(c->Zstdm-c->Zsm)*(Zxm-c->Zsm);
	return Zx;	
}
