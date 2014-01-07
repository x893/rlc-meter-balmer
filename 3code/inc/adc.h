// balmer@inbox.ru 2013 RLC Meter

typedef struct {
	uint16_t adc_min;
	uint16_t adc_max;
	uint16_t count;
	int64_t sin_sum;
	int64_t cos_sum;
	uint32_t mid_sum;
} AdcSummaryChannel;


typedef struct {
	AdcSummaryChannel ch_v;
	AdcSummaryChannel ch_i;
	bool error;
	uint32_t nop_number;
} AdcSummaryData;

void AdcInit();


void AdcStartReadBuffer();
void AdcReadBuffer();

void AdcDacStartSynchroUsb(uint32_t period, uint8_t num_skip);

void AdcQuant();

void AdcSendLastCompute();

extern uint16_t g_adcStatus;
extern bool g_adc_read_buffer;
extern uint32_t g_adc_elapsed_time;
