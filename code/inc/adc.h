
void AdcInit();
void AdcStart();


void AdcStartReadBuffer();
void AdcReadBuffer();

void AdcDacStartSynchro(uint32_t frequency, uint8_t num_skip);

extern uint16_t g_adcStatus;
extern bool g_adc_read_buffer;
extern uint32_t g_adc_elapsed_time;
