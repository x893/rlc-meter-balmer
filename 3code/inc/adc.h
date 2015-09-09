// balmer@inbox.ru RLC Meter 303
// 2013-2014

#ifndef _ADC_H_
#define _ADC_H_

#include <stdint.h>
#include <stdbool.h>

#define RESULT_BUFFER_SIZE 2000

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

typedef struct ADC_Context_s {
			uint32_t g_ResultBufferSize;
			uint32_t g_adc_elapsed_time;
			uint16_t g_adcStatus;
			uint16_t g_adc_cur_read_pos;
	volatile uint8_t g_adc_cycles;
	volatile uint8_t g_cur_cycle;
	volatile bool g_usb_request_data;
	volatile bool g_usb_sampled_data;
			bool g_adc_read_buffer;
} ADC_Context_t;

extern ADC_Context_t ADC_Context;

extern uint32_t g_resultBufferCopy[RESULT_BUFFER_SIZE];
extern AdcSummaryData g_data;

void AdcInit(void);
void AdcDacStartSynchro(uint32_t period, uint16_t amplitude);
void AdcQuant(void);
void AdcSendLastCompute(void);

// usb functions
void AdcUsbRequestData(void);
bool AdcUsbBufferComplete(void);
void AdcUsbStartReadBuffer(void);
void AdcUsbReadBuffer(void);

//Calc functions
void AdcClearData(AdcSummaryData* data);
void AdcCalcData(AdcSummaryData* data, uint16_t* inV, uint16_t* inI, uint16_t count);

#endif//_ADC_H_
