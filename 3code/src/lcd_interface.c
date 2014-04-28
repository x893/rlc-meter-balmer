#include "hw_config.h"
#include <math.h>
#include "lcd_interface.h"
#include "format_print.h"
#include "calc_rc.h"
#include "dac.h"

int printDelta = 0;
float Rre = 0;
float Rim = 0;
extern uint32_t corrSize;

void printLcdFrequency()
{
    float f = DacFrequency();
    if(f<999.5f)
    {
        printInt(round(f), FONT_1X);
        LcdStr(FONT_1X, "Hz");
        return;
    }

    if(f<1e6f)
    {
        int32_t fi = round(f*1e-3f);
        if(fi*1000==f)
        {
            printInt(fi, FONT_1X);
        } else
        {
            printIntFixed(round(f*1e-2f), FONT_1X, 2, 1);
        }

        LcdStr(FONT_1X, "KHz");
        return;
    }

    printF(f);
}

void LcdRepaint()
{
    LcdClear();

    LcdGotoXYFont(1,1);
    printLcdFrequency();
    printRX2(Rre, 2);
    printRX2(Rim, 4);
    LcdGotoXYFont(12,2);
    LcdStr(FONT_1X, "Rre");
    LcdGotoXYFont(12,4);
    LcdStr(FONT_1X, "Rim");
    //LcdGotoXYFont(1,4);
    //LcdStr(FONT_1X, "re=");
    //printR(Rre, FONT_1X);
    //LcdGotoXYFont(1,5);
    //LcdStr(FONT_1X, "im=");
    //printR(Rim, FONT_1X);

    LcdGotoXYFont(1,6);
    LcdStr(FONT_1X, "D=");
    //printInt(printDelta, FONT_1X);
    printInt(corrSize, FONT_1X);
    LcdGotoXYFont(9,6);
    LcdStr(FONT_1X, "R");
    printInt(resistorIdx, FONT_1X);
    LcdStr(FONT_1X, "V");
    printInt(gainVoltageIdx, FONT_1X);  
    LcdStr(FONT_1X, "I");
    printInt(gainCurrentIdx, FONT_1X);
    LcdUpdate();
}
