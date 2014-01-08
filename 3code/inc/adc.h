// balmer@inbox.ru 2013 RLC Meter

typedef struct {
	uint16_t adc_min;
	uint16_t adc_max;
	uint16_t count;
	float sin_sum;
	float cos_sum;
	uint32_t mid_sum;
} AdcSummaryChannel;


typedef struct {
	AdcSummaryChannel ch_v;
	AdcSummaryChannel ch_i;
	bool error;
	uint32_t nop_number;
} AdcSummaryData;

void AdcInit();



void AdcDacStartSynchro(uint32_t period);

void AdcQuant();

void AdcSendLastCompute();

extern uint16_t g_adcStatus;
extern bool g_adc_read_buffer;
extern uint32_t g_adc_elapsed_time;

//usb functions
void AdcUsbRequestData();
bool AdcUsbBufferComplete();
void AdcUsbStartReadBuffer();
void AdcUsbReadBuffer();

//