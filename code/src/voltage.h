

void voltageInit(void);

enum VOLTAGE_ENUM
{
	VoltageOff,
	VoltageZero,
	VoltageUp1,
	VoltageUp2,
	VoltageUp3,
};

#define ADC_BUFFER_SIZE 1024
extern uint16_t adc_buffer[ADC_BUFFER_SIZE];
extern volatile uint8_t g_start_conversion;

void voltageSet(enum VOLTAGE_ENUM type);

void USBCommandReceive(uint8_t* commandBuffer, uint16_t commandSize);

void USBAdd(uint8_t* data, uint32_t size);
void USBAddStr(char* data);
void USBAdd8(uint8_t data);
void USBAdd16(uint16_t data);
void USBAdd32(uint32_t data);
void USBSend(void);

void ReceiveADCData(void);

extern uint8_t adc_get_result_command;
void SendConversionResult();

void StartTimer(void);
void StopTimer(void);
uint32_t GetTime(void);