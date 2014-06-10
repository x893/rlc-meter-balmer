// balmer@inbox.ru RLC Meter 303
// 2013-2014

#ifndef _DAC_H_
#define _DAC_H_

#define pi  3.14159f
#define SINUS_BUFFER_SIZE 1000
#define DEFAULT_DAC_AMPLITUDE 1200

extern float g_sinusBufferFloat[SINUS_BUFFER_SIZE];

void DacInit(void);

/*
	Stop DAC and set frequency
*/
void DacSetFrequency(uint32_t frequency);
void DacSetPeriod(uint32_t sinusPeriod, uint16_t amplitude);
void DacStart();

uint32_t DacPeriod(void);
float DacFrequency();
uint32_t DacSamplesPerPeriod(void);
uint32_t DacSampleTicks(void);

#endif//_DAC_H_