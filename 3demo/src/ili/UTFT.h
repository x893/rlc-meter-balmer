/*
 ILI9341 2.2 TFT SPI library
 based on UTFT.cpp - Arduino/chipKit library support for Color TFT LCD Boards
 Copyright (C)2010-2013 Henning Karlsen. All right reserved
 
 Compatible with other UTFT libraries.
 
 Original library you can find at http://electronics.henningkarlsen.com/
  
 This library is free software; you can redistribute it and/or
 modify it under the terms of the CC BY-NC-SA 3.0 license.
 Please see the included documents for further information.
 */

#ifndef UTFT_h
#define UTFT_h

#define UTFT_VERSION	270

#define UTFT_LEFT 0
#define UTFT_RIGHT 9999
#define UTFT_CENTER 9998

#define UTFT_PORTRAIT 0
#define UTFT_LANDSCAPE 1

//*********************************
// COLORS
//*********************************
// VGA color palette
#define VGA_BLACK		0x0000
#define VGA_WHITE		0xFFFF
#define VGA_RED			0xF800
#define VGA_GREEN		0x0400
#define VGA_BLUE		0x001F
#define VGA_SILVER		0xC618
#define VGA_GRAY		0x8410
#define VGA_MAROON		0x8000
#define VGA_YELLOW		0xFFE0
#define VGA_OLIVE		0x8400
#define VGA_LIME		0x07E0
#define VGA_AQUA		0x07FF
#define VGA_TEAL		0x0410
#define VGA_NAVY		0x0010
#define VGA_FUCHSIA		0xF81F
#define VGA_PURPLE		0x8010
#define VGA_TRANSPARENT	0xFFFFFFFF

#ifndef INT8U
#define INT8U uint8_t
#endif
#ifndef INT16U
#define INT16U uint16_t
#endif

typedef uint8_t byte;
typedef uint16_t word;

#define bitmapdatatype uint16_t*
#define swap(type, i, j) {type t = i; i = j; j = t;}
#define fontbyte(x) (cfont.font[x])

typedef struct _current_font
{
	uint8_t* font;
	uint8_t x_size;
	uint8_t y_size;
	uint8_t offset;
	uint8_t numchars;
} _current_font;

//LANDSCAPE default
void UTFT_InitLCD(byte orientation);
void UTFT_clrScr();
void UTFT_drawPixel(int x, int y);
void UTFT_drawLine(int x1, int y1, int x2, int y2);
void UTFT_fillScr(byte r, byte g, byte b);
void UTFT_fillScrW(word color);
void UTFT_drawRect(int x1, int y1, int x2, int y2);
void UTFT_drawRoundRect(int x1, int y1, int x2, int y2);
void UTFT_fillRect(int x1, int y1, int x2, int y2);
void UTFT_fillRoundRect(int x1, int y1, int x2, int y2);
void UTFT_drawCircle(int x, int y, int radius);
void UTFT_fillCircle(int x, int y, int radius);
void UTFT_setColor(byte r, byte g, byte b);
void UTFT_setColorW(word color);
word UTFT_getColor();
void UTFT_setBackColor(byte r, byte g, byte b);
void UTFT_setBackColorW(uint32_t color);
word UTFT_getBackColor();

//deg=0
void UTFT_print(char *st, int x, int y, int deg);

//length=0, char filler=' '
void UTFT_printNumI(long num, int x, int y, int length, char filler);

//divider='.', length=0, filler=' '
void UTFT_printNumF(double num, byte dec, int x, int y, char divider, int length, char filler);
void UTFT_setFont(uint8_t* font);
uint8_t* UTFT_getFont();
uint8_t UTFT_getFontXsize();
uint8_t UTFT_getFontYsize();

//scale=1
void UTFT_drawBitmap(int x, int y, int sx, int sy, bitmapdatatype data, int scale);
void UTFT_drawBitmapR(int x, int y, int sx, int sy, bitmapdatatype data, int deg, int rox, int roy);
void UTFT_lcdOff();
void UTFT_lcdOn();
void UTFT_setContrast(char c);
int  UTFT_getDisplayXSize();
int	 UTFT_getDisplayYSize();

#endif