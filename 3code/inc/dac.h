#ifndef _DAC_H_
#define _DAC_H_

#define pi  3.14159f
#define SINUS_BUFFER_SIZE 1000

#define MUL_BUFFER_FLOAT 4096
extern int16_t g_sinusBufferFloat[SINUS_BUFFER_SIZE];

void DacInit(void);

/*
	Stop DAC and set frequency
*/
void DacSetFrequency(uint32_t frequency);
void DacSetPeriod(uint32_t period);
void DacStart();

uint32_t DacPeriod(void);
uint32_t DacSamplesPerPeriod(void);
uint32_t DacSampleTicks(void);

#endif//_DAC_H_