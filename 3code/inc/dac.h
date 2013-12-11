
void DacInit(void);

/*
	Stop DAC and set frequency
*/
void DacSetFrequency(uint32_t frequency);
void DacSetPeriod(uint32_t period);
void DacStart();

uint32_t DacPeriod(void);
