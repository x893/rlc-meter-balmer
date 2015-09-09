// balmer@inbox.ru RLC Meter 303
// 2013-2014

#ifndef _PROCESS_MEASURE_H_
#define _PROCESS_MEASURE_H_

#include "adc.h"

typedef enum STATES
{
	STATE_NOP = 0,
	STATE_INIT_WAIT,
	STATE_RESISTOR_INDEX,
	STATE_RESISTOR_INDEX_WAIT,
	STATE_GAIN_INDEX,
	STATE_GAIN_INDEX_WAIT,
	STATE_MEASURE,
	STATE_MEASURE_WAIT,
} STATES;

void ProcessData(void);
uint16_t ProcessCalcOptimalCount(void);
void ProcessStartComputeX(
	uint16_t count,
	uint8_t resistorIdx,
	uint8_t gainVoltageIdx,
	uint8_t gainCurrentIdx,
	bool useCorrector
	);
STATES ProcessGetState(void);
void SendRVI(void);

//!!! extern bool isSerial;

typedef struct Measure_Context_s {
	const uint8_t * gainIdxPtr;

	uint16_t computeXCount;
	uint16_t computeXIterator;

	STATES state;
	uint8_t predefinedResistorIdx;
	uint8_t predefinedGainVoltageIdx;
	uint8_t predefinedGainCurrentIdx;
	uint8_t resistorIdx;
	uint8_t gainVoltageIdx;
	uint8_t gainCurrentIdx;
	uint8_t gainIdxCount;
	uint8_t gainIndexIterator;
	uint8_t initWaitCount;

	bool gainIndexStopV;
	bool gainIndexStopI;
	bool useCorrector;
	bool bContinuousMode;
	bool bCalibration;
	bool debugRepaint;

} Measure_Context_t;

extern Measure_Context_t Measure_Context;

float getGainValue(uint8_t idx);
float getGainValueV(void);
float getGainValueI(void);
float getResistorOm(void);

#endif//_PROCESS_MEASURE_H_
