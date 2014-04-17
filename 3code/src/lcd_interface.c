#include "hw_config.h"
#include "lcd_interface.h"
#include "calc_rc.h"

static uint32_t frequency = 0;
int printDelta = 0;

void LcdFrequency(uint32_t f)
{
	frequency = f;
}

void LcdRepaint()
{
  LcdClear();
  LcdGotoXYFont(1,1);
  LcdStr(FONT_1X, "D=");
  printInt(printDelta, FONT_1X);

  LcdGotoXYFont(1,2);
  LcdStr(FONT_1X, "F=");
  printInt(frequency, FONT_1X);
  LcdGotoXYFont(1,3);
  LcdStr(FONT_1X, "R=");
  printInt(resistorIdx, FONT_1X);
  LcdUpdate();
}
