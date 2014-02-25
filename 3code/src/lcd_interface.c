#include "lcd_interface.h"

static uint32_t frequency = 0;

void LcdFrequency(uint32_t f)
{
	frequency = f;
}

void LcdRepaint()
{
  LcdClear();
  LcdGotoXYFont(1,2);
  LcdStr(FONT_1X, "F=");
  printInt(frequency, FONT_1X);
  LcdUpdate();
}
