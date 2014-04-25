//
//  complex.cpp
//  RLC Meter
//
//  Created by Balmer on 13.04.14.
//
//

#include "hw_config.h"
#include <math.h>
#include <stdbool.h>
#include "voltage.h"
#include "calc_rc.h"
#include "mcp6s21.h"
#include "lcd_interface.h"
#include "corrector.h"

extern float Rre, Rim;

void OnCalculate()
{
	float gain_V = getGainValueV();
	float gain_I = getGainValueI();
	float resistor = getResistorOm();
	float toVolts = 3.3f/4095.0f;
	complexf zV = g_data.ch_v.k_sin + g_data.ch_v.k_cos*I;
	complexf zI = g_data.ch_i.k_sin + g_data.ch_i.k_cos*I;
	zV *= toVolts/gain_V;
	zI *= toVolts/gain_I;

	complexf R = (zV/zI)*resistor;
	R *= GainCorrector(gainVoltageIdx, gainCurrentIdx);

	R = Corrector(R);

	Rre = creal(R);
	Rim = cimag(R);
}