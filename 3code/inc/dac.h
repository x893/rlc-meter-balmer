#ifndef _DAC_H_
#define _DAC_H_

#define pi  3.14159f
#define SINUS_BUFFER_SIZE 1000

extern float g_sinusBufferFloat[SINUS_BUFFER_SIZE];

void DacInit(void);

/*
	Stop DAC and set frequency
*/
void DacSetFrequency(uint32_t frequency);
void DacSetPeriod(uint32_t sinusPeriod, uint16_t amplitude);
void DacStart();

uint32_t DacPeriod(void);
uint32_t DacSamplesPerPeriod(void);
uint32_t DacSampleTicks(void);

#endif//_DAC_H_