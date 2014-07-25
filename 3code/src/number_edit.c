#include "hw_config.h"
#include "lcd_interface.h"
#include "menu.h"
#include "format_print.h"

#include "number_edit.h"

void NumberEditRepaint();

static bool started = false;
static bool completed = false;
static float value = 0.1;
static const char* text = NULL;
int powMinDelta = -1;
int powMaxDelta = 1;
int powCurDelta = 0;

void NumberEditStart()
{
	started = true;
	powCurDelta = powMaxDelta;
}

void NumberEditEnd()
{
	completed = false;
	started = false;
}

float NumberEditGetValue()
{
	return value;
}

bool NumberEditStarted()
{
	return started;
}

bool NumberEditCompleted()
{
	return completed;
}

void NumberEditSetText(const char* atext)
{
	text = atext;
}

void NumberEditSetValue(float avalue, int apowMinDelta, int apowMaxDelta)
{
	value = avalue;
	powMinDelta = apowMinDelta;	
	powMaxDelta = apowMaxDelta;
}

void NumberEditOnButtonPressed()
{
	if(powCurDelta>powMinDelta)
	{
		powCurDelta--;
		return;
	}

	completed = true;
}

float GetCurDelta()
{
	float f = 1.0f;
	if(powCurDelta>0)
	{
		for(int i=0; i<powCurDelta; i++)
			f *= 10.0f;
	} else
	{
		for(int i=0; i<-powCurDelta; i++)
			f *= 0.1f;
	}
	return f;
}

void NumberEditOnWeel(int16_t delta)
{
	value += delta*GetCurDelta();
}

void NumberEditRepaint()
{
	if(text)
	{
		LcdGotoXYFont(1,1);
		LcdStr(FONT_1X, text);
	}

	printRX2(value, 2);

    LcdGotoXYFont(1,6);
    LcdStr(FONT_1X, "+-");
    printR(GetCurDelta(), FONT_1X);
}
