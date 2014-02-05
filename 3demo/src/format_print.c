#include "pcd8544.h"
char Buffer[32];

void addCommaToBuffer(uint8_t posComma)
{
	uint8_t size = 0;
	for(;size<sizeof(Buffer) && Buffer[size]; size++);

	if(size>=posComma)
	{
		for(uint8_t i=size; i>size-posComma; i--)
		{
			Buffer[i] = Buffer[i-1];
		}

		Buffer[size+1] = 0;
		Buffer[size-posComma] = '.';
	}
}

void sprintIntFormat(int16_t value, uint8_t aMinDigits, uint8_t aEmptyChar)
{
	char* buf = Buffer;
    uint16_t valueUnsigned;
	if(value<0)
	{
		*buf++ = '-';
		valueUnsigned = -value;
	} else
    {
        valueUnsigned = value;
    }
    
	int8_t digits = 0;
	for(uint16_t v=valueUnsigned; v>0; digits++)
	{
		v/=10;
	}
    
	if(value==0)
	{
		digits = 1;
	}
    
	for(int8_t i=digits; i<aMinDigits; i++)
	{
		*buf++ = aEmptyChar;
	}
	
	buf[digits--]=0;
	for(uint16_t v=valueUnsigned; digits>=0; digits--)
	{
		buf[digits] = (v%10)+'0';
		v /= 10;
	}
}

void printIntFormat(int16_t value, uint8_t font, uint8_t aMinDigits, uint8_t aEmptyChar)
{
	sprintIntFormat(value, aMinDigits, aEmptyChar);
	LcdStr(font, Buffer);
}

void printIntFixed(int16_t value, uint8_t font, uint8_t aMinDigits, uint8_t aFixedPoint)
{
	sprintIntFormat(value, aMinDigits, '0');
	addCommaToBuffer(aFixedPoint);
	LcdStr(font, Buffer);
}

void printInt(int16_t value, uint8_t font)
{
	printIntFormat(value, font, 1, ' ');
}
