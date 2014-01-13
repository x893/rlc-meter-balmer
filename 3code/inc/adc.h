// balmer@inbox.ru 2013 RLC Meter

#define RESULT_BUFFER_SIZE 3000

typedef struct {
	uint16_t adc_min;
	uint16_t adc_max;
	float k_sin;
	float k_cos;
	float adc_mid;
	float square_error;
} AdcSummaryChannel;


typedef struct {
	uint16_t count;
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

extern uint32_t g_resultBufferCopy[RESULT_BUFFER_SIZE];
extern uint32_t g_ResultBufferSize;

//usb functions
void AdcUsbRequestData();
bool AdcUsbBufferComplete();
void AdcUsbStartReadBuffer();
void AdcUsbReadBuffer();

//Calc functions
void AdcClearData(AdcSummaryData* data);
void AdcAddData(AdcSummaryData* data, uint16_t* inV, uint16_t* inI, uint16_t count);
