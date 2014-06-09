// balmer@inbox.ru RLC Meter 303
// 2013-2014
#include "hw_config.h"
#include <math.h>
#include "adc.h"
#include "dac.h"
#include "voltage.h"
#include "usb_desc.h"
#include "systick.h"

void AdcClearChData(AdcSummaryChannel* ch)
{
	ch->adc_min = 0xFFFF;
	ch->adc_max = 0;
	ch->k_sin = 0.1f;
	ch->k_cos = 0.2f;
	ch->adc_mid = 0;
	ch->square_error = 0.123f;
}

void AdcClearData(AdcSummaryData* data)
{
	data->count = 0;
	AdcClearChData(&data->ch_v);
	AdcClearChData(&data->ch_i);
	data->error = false;
	data->nop_number = 33;
}

static void CalcSquareError(AdcSummaryChannel* ch, uint16_t* in, uint16_t count)
{
	uint16_t nsamples = DacSamplesPerPeriod();
	uint16_t nsamples4 = nsamples>>2;

	float sum_error = 0;
	for(uint16_t i=0; i<count; i++)
	{
		float sin_table = g_sinusBufferFloat[i%nsamples];
		float cos_table = g_sinusBufferFloat[(i+nsamples4)%nsamples];

		float d = ch->adc_mid + sin_table*ch->k_sin + cos_table*ch->k_cos;
		d -= in[i];

		sum_error += d*d;
	}

	ch->square_error = sqrt(sum_error/(count-1));
}

void AdcCalcData(AdcSummaryData* data, uint16_t* inV, uint16_t* inI, uint16_t count)
{
	uint16_t nsamples = DacSamplesPerPeriod();
	uint16_t nsamples4 = nsamples>>2;

	float sin_v = 0;
	float cos_v = 0;
	float sin_i = 0;
	float cos_i = 0;

	data->ch_v.k_sin = 1;
	data->ch_v.k_cos = 2;
	data->ch_i.k_sin = 3;
	data->ch_i.k_cos = 4;

	uint32_t mid_sum_v = 0;
	uint32_t mid_sum_i = 0;

	for(uint16_t i=0; i<count; i++)
	{
		{
			uint16_t cV = inV[i];
			if(cV < data->ch_v.adc_min)
				data->ch_v.adc_min = cV;
			if(cV > data->ch_v.adc_max)
				data->ch_v.adc_max = cV;

			mid_sum_v += cV;

		}

		{
			uint16_t cI = inI[i];
			if(cI < data->ch_i.adc_min)
				data->ch_i.adc_min = cI;
			if(cI > data->ch_i.adc_max)
				data->ch_i.adc_max = cI;

			mid_sum_i += cI;

		}
	}

	float mid_v = mid_sum_v/(float)count;
	float mid_i = mid_sum_i/(float)count;

	data->ch_v.adc_mid = mid_v;
	data->ch_i.adc_mid = mid_i;

	for(uint16_t i=0; i<count; i++)
	{
		float sin_table = g_sinusBufferFloat[i%nsamples];
		float cos_table = g_sinusBufferFloat[(i+nsamples4)%nsamples];

		{
			float cV = inV[i]-mid_v;
			sin_v += cV * sin_table;
			cos_v += cV * cos_table;
		}

		{
			float cI = inI[i]-mid_i;
			sin_i += cI * sin_table;
			cos_i += cI * cos_table;
		}
	}

	data->ch_v.k_sin = sin_v*2.0f/(float)count;
	data->ch_v.k_cos = cos_v*2.0f/(float)count;
	data->ch_i.k_sin = sin_i*2.0f/(float)count;
	data->ch_i.k_cos = cos_i*2.0f/(float)count;

	CalcSquareError(&data->ch_v, inV, count);
	CalcSquareError(&data->ch_i, inI, count);

}
