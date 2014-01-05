// balmer@inbox.ru 2013 RLC Meter

void AdcInit();


void AdcStartReadBuffer();
void AdcReadBuffer();

void AdcDacStartSynchro(uint32_t period, uint8_t num_skip, uint16_t average_number);

void AdcQuant();

extern uint16_t g_adcStatus;
extern bool g_adc_read_buffer;
extern uint32_t g_adc_elapsed_time;
