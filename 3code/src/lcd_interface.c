// balmer@inbox.ru RLC Meter 303
// 2013-2014

#include "hw_config.h"
#include <math.h>
#include "lcd_interface.h"
#include "format_print.h"
#include "calc_rc.h"
#include "dac.h"
#include "menu.h"
#include "vbat.h"
#include "corrector.h"

int printD = 0; // debug
float printGradus = -1000;

float Rre = 0;
float Rim = 0;

bool printRim = false;
bool isSerial = true;
bool valueIsC = true;
bool calculatedValues = false;
float valueL = 0;
float valueC = 0;


void printLcdFrequency()
{
	float f = DacFrequency();
	if (f < 999.5f)
	{
		printInt(round(f), FONT_1X);
		LcdStr(FONT_1X, "Hz");
		return;
	}

	if (f < 1e6f)
	{
		int32_t fi = round(f*1e-3f);
		if (fi * 1000 == f)
		{
			printInt(fi, FONT_1X);
		}
		else
		{
			printIntFixed(round(f*1e-2f), FONT_1X, 2, 1);
		}

		LcdStr(FONT_1X, "KHz");
		return;
	}
	printF(f);
}

void printLcdGradus()
{
	float f = printGradus;
	LcdStr(FONT_1X, "D=");
	if (f > -100 && f < +100)
	{
		printIntFixed(round(f * 10), FONT_1X, 2, 1);
		LcdStr(FONT_1X, "{");
		return;
	}
	else
	{
		LcdStr(FONT_1X, "---");
	}
}

void LcdRepaint()
{
	LcdClear();
	if (MenuIsOpen())
	{
		MenuRepaint();
		LcdUpdate();
		return;
	}

	LcdGotoXYFont(1, 1);
	printLcdFrequency();

	LcdGotoXYFont(8, 1);
	LcdStr(FONT_1X, isSerial ? "SER" : "PAR");


	if (calculatedValues)
	{
		printRX2(Rre, 2);
		LcdGotoXYFont(12, 2);
		LcdStr(FONT_1X, "Rre");

		if (printRim)
		{
			printRX2(Rim, 4);
			LcdGotoXYFont(12, 4);
			LcdStr(FONT_1X, "Rim");
		}
		else
		{
			if (valueIsC)
			{
				printCX2(valueC, 4);
				LcdGotoXYFont(12, 4);
				LcdStr(FONT_1X, "C");
			}
			else
			{
				printLX2(valueL, 4);
				LcdGotoXYFont(12, 4);
				LcdStr(FONT_1X, "L");
			}
		}
	}

	VBatQuant();

	LcdGotoXYFont(1, 6);

//	LcdStr(FONT_1X, "D=");
//	printInt(printD, FONT_1X);

	if (Measure_Context.bCalibration)
	{
		LcdStr(FONT_1X, "CAL");
	}
	else
	{
		printLcdGradus();
	}

	LcdGotoXYFont(9, 6);
	LcdStr(FONT_1X, "R");
	printInt(Measure_Context.resistorIdx, FONT_1X);
	LcdStr(FONT_1X, "V");
	printInt(Measure_Context.gainVoltageIdx, FONT_1X);
	LcdStr(FONT_1X, "I");
	printInt(Measure_Context.gainCurrentIdx, FONT_1X);

	if (GetCorrector()->period == 0)
	{
		LcdSingleBar(0, 8, 9, 6 * 7, PIXEL_XOR);
	}

	LcdUpdate();
}

__weak void LcdDrawBattery(int32_t value)
{
	/*
	// Draw battery
	uint8_t x0 = 63, y0 = 0;
	LcdLine(x0, x0, y0 + 1, y0 + 7, PIXEL_ON);
	LcdLine(x0 + 1, x0 + 3, y0 + 1, y0 + 1, PIXEL_ON);
	LcdLine(x0 + 1, x0 + 3, y0 + 7, y0 + 7, PIXEL_ON);

	LcdLine(x0 + 3, x0 + 3, y0 + 1, y0 + 0, PIXEL_ON);
	LcdLine(x0 + 3, x0 + 3, y0 + 7, y0 + 8, PIXEL_ON);

	LcdLine(x0 + 3, x0 +19, y0 + 0, y0 + 0, PIXEL_ON);
	LcdLine(x0 + 3, x0 +19, y0 + 8, y0 + 8, PIXEL_ON);
	LcdLine(x0 +20, x0 +20, y0 + 0, y0 + 8, PIXEL_ON);

	if (value > 5)
		LcdSingleBar(x0 + 2, y0 + 3 + 3, 3, 2, PIXEL_ON);

	for (int i = 0; i < 5; i++)
		if (value > 4 - i)
			LcdSingleBar(x0 + 5 + i * 3, y0 + 3 + 4, 5, 2, PIXEL_ON);
	*/
}

void LcdHello()
{
	/*
	LcdClear();
	LcdGotoXYFont(3, 3);
	LcdStr(FONT_2X, "RLC");

	LcdGotoXYFont(9, 3);
	LcdStr(FONT_1X, "meter");

	LcdGotoXYFont(3, 5);
	LcdStr(FONT_1X, "BALMER 303{");

	LcdUpdate();
	*/
}
