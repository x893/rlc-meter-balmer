// balmer@inbox.ru RLC Meter 303
// 2013-2014

void USBCommandReceive(uint8_t* commandBuffer, uint16_t commandSize);

void USBAdd(uint8_t* data, uint32_t size);
void USBAddStr(char* data);
void USBAdd8(uint8_t data);
void USBAdd16(uint16_t data);
void USBAdd32(uint32_t data);
void USBAddFloat(float data);
void USBSend(void);


void StartTimer(void);
void StopTimer(void);
uint32_t GetTime(void);