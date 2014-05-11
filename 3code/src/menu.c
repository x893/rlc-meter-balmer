#include "hw_config.h"
#include "quadrature_encoder.h"
#include "lcd_interface.h"
#include "menu.h"
#include "dac.h"
#include "adc.h"
#include "process_measure.h"

#define SIZEOF(x) (sizeof(x)/sizeof(x[0]))

typedef enum MenuEnum {
	MENU_MAIN_RETURN,
	MENU_MAIN_FREQUENCY,
	MENU_MAIN_SER_PAR,
	MENU_MAIN_VIEW_PARAM, //Какой из параметров отображается.
	MENU_F_RETURN,
	MENU_F_100Hz,
	MENU_F_1KHz,
	MENU_F_10KHz,
	MENU_F_93_75KHz,
	MENU_SP_RETURN,
	MENU_SP_SERIAL,
	MENU_SP_PARALLEL,
} MenuEnum;

typedef struct MenuElem {
	char* text;
	MenuEnum command;
} MenuElem;

static MenuElem g_main_menu[]={
	{"..", MENU_MAIN_RETURN},
	{"Frequency", MENU_MAIN_FREQUENCY},
	{"SER/PAR", MENU_MAIN_SER_PAR},
	{"View", MENU_MAIN_VIEW_PARAM},
};

static MenuElem g_f_menu[]={
	{"..", MENU_F_RETURN},
	{"100 Hz", MENU_F_100Hz},
	{"1 KHz", MENU_F_1KHz},
	{"10 KHz", MENU_F_10KHz},
	{"93.75 KHz", MENU_F_93_75KHz},
};

static MenuElem g_sp_menu[]={
	{"..", MENU_SP_RETURN},
	{"SERIAL", MENU_SP_SERIAL},
	{"PARALLEL", MENU_SP_PARALLEL},
};

static MenuElem* g_cur_menu = NULL;
static uint8_t g_menu_size = 0;
static uint8_t g_menu_pos = 0;
static bool g_update = false;

#define MENU_START(menu) \
	g_cur_menu = menu; \
	g_menu_size = SIZEOF(menu); \
	g_menu_pos = 0;

#define MENU_CLEAR() \
	g_cur_menu = NULL; \
	g_menu_size = 0;

void MenuSetF(uint32_t period);
void MenuSetSerial(bool ser);

void OnButtonPressed()
{
	g_update = true;
	if(g_cur_menu==NULL)
	{
		MENU_START(g_main_menu);
		return;
	}

	if(g_menu_pos<g_menu_size)
	switch(g_cur_menu[g_menu_pos].command)
	{
	case MENU_MAIN_RETURN:
		MENU_CLEAR();
		break;

	case MENU_MAIN_FREQUENCY:
		MENU_START(g_f_menu);
		break;
	case MENU_MAIN_SER_PAR:
		MENU_START(g_sp_menu);
		break;
	case MENU_MAIN_VIEW_PARAM:
		break;
	case MENU_F_RETURN:
	case MENU_SP_RETURN:
		MENU_START(g_main_menu);
		break;
	case MENU_F_100Hz:
		MenuSetF(720000);
		break;
	case MENU_F_1KHz:
		MenuSetF(72000);
		break;
	case MENU_F_10KHz:
		MenuSetF(7200);
		break;
	case MENU_F_93_75KHz:
		MenuSetF(768);
		break;
	case MENU_SP_SERIAL:
		MenuSetSerial(true);
		break;
	case MENU_SP_PARALLEL:
		MenuSetSerial(false);
		break;
	}
}

void OnWeel(int16_t delta)
{
	if(g_cur_menu==NULL)
		return;

	g_menu_pos = (g_menu_pos+g_menu_size-delta)%g_menu_size;
    g_update = true;
}

void OnTimer()
{
	if(g_update)
	{
		LcdRepaint();
		g_update = false;		
	}
}

void MenuRepaint()
{
	if(g_menu_size==0 || g_cur_menu==NULL)
		return;
	const byte height = 6;
	const byte font_height = 8;
	byte ystart = (height-g_menu_size)/2+1;
	for(uint8_t i=0; i<g_menu_size; i++)
	{
		LcdGotoXYFont( 2, ystart+i );
		LcdStr(FONT_1X, g_cur_menu[i].text);

	}

	if(g_menu_pos<g_menu_size)
	{
		LcdSingleBar( 4, (ystart+g_menu_pos)*font_height+1, font_height+1, 6*12+4, PIXEL_XOR );
	}
}

bool MenuIsOpen()
{
	return g_menu_size!=0 && g_cur_menu!=NULL;
}


void MenuSetF(uint32_t period)
{
	bContinuousMode = true;
	AdcDacStartSynchro(period, DEFAULT_DAC_AMPLITUDE);
	ProcessStartComputeX(10, 255);
	MENU_CLEAR();
}

void MenuSetSerial(bool ser)
{
	isSerial = ser;
	MENU_CLEAR();
}
