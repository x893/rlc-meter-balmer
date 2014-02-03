void PressureInit(void);
uint16_t PressureRead(uint8_t reg);
void PressureWrite(uint8_t reg, uint8_t value);

/*
	Read temperature fixed digit
	135 -> 13.5 C
*/
int16_t PressureReadTemp();

//return mbar
uint16_t PressureReadPressure();
