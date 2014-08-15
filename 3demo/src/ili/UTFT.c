/*
  ILI9341 2.2 TFT SPI library
  based on UTFT.cpp - Arduino/chipKit library support for Color TFT LCD Boards
  Copyright (C)2010-2013 Henning Karlsen. All right reserved
 
  Compatible with other UTFT libraries.
 
  Original library you can find at http://electronics.henningkarlsen.com/
 
 Pinout:
 
 (Arduino UNO : TFT)
    D4  : RESET
    D5  : CS
    D6  : D/C
    D7  : LED
    D11 : MOSI
    D12 : MISO
    D13 : SCK
 
  This library is free software; you can redistribute it and/or
  modify it under the terms of the CC BY-NC-SA 3.0 license.
  Please see the included documents for further information.
*/

#include <stdbool.h>
#include <string.h>
#include <math.h>

#include "hw_ili9341.h"
#include "UTFT.h"

#define TFT_CS_LOW  HwLcdPinCE(0)
#define TFT_CS_HIGH HwLcdPinCE(1)
#define TFT_DC_LOW  HwLcdPinDC(0)
#define TFT_DC_HIGH HwLcdPinDC(1)
#define TFT_BL_OFF  HwLcdPinLed(0)
#define TFT_BL_ON   HwLcdPinLed(1)
#define TFT_RST_OFF HwLcdPinRst(1)
#define TFT_RST_ON  HwLcdPinRst(0)


/*
	The functions and variables below should not normally be used.
	They have been left publicly available for use in add-on libraries
	that might need access to the lower level functions of UTFT.

	Please note that these functions and variables are not documented
	and I do not provide support on how to use them.
*/
static byte fch, fcl, bch, bcl;
static byte orient;
static long disp_x_size=239, disp_y_size=319;
//byte display_model, display_transfer_mode, display_serial_mode;
//regtype *P_RS, *P_WR, *P_CS, *P_RST, *P_SDA, *P_SCL, *P_ALE;
//regsize B_RS, B_WR, B_CS, B_RST, B_SDA, B_SCL, B_ALE;
static _current_font	cfont;
static bool _transparent;

void UTFT_setPixel(word color);
void UTFT_drawHLine(int x, int y, int l);
void UTFT_drawVLine(int x, int y, int l);
void UTFT_printChar(byte c, int x, int y);
void UTFT_setXY(word x1, word y1, word x2, word y2);
void UTFT_clrXY();
void UTFT_rotateChar(byte c, int x, int y, int pos, int deg);
//void UTFT_convert_float(char *buf, double num, int width, byte prec);
static void sendCMD(INT8U VL);
static void WRITE_DATA(INT8U VL);
INT8U UTFT_readID(void);
INT8U UTFT_Read_Register(INT8U Addr, INT8U xParameter);

void Delay(uint32_t nTime);

void delay(uint32_t nTime)
{
	nTime = nTime/10;
	if(nTime==0)
		nTime = 1;
	Delay(nTime);
}

static void sendCMD(INT8U index)
{
    TFT_DC_LOW;
    TFT_CS_LOW;
    HwLcdSend(index);
    TFT_CS_HIGH;
}

static void WRITE_DATA(INT8U data)
{
    TFT_DC_HIGH;
    TFT_CS_LOW;
    HwLcdSend(data);
    TFT_CS_HIGH;
}

/*
static void sendData(INT16U data)
{
    INT8U data1 = data>>8;
    INT8U data2 = data&0xff;
    TFT_DC_HIGH;
    TFT_CS_LOW;
    HwLcdSend(data1);
    HwLcdSend(data2);
    TFT_CS_HIGH;
}

static void WRITE_Package(INT16U *data, INT8U howmany)
{
    INT16U    data1 = 0;
    INT8U   data2 = 0;
    
    TFT_DC_HIGH;
    TFT_CS_LOW;
    INT8U count=0;
    for(count=0;count<howmany;count++)
    {
        data1 = data[count]>>8;
        data2 = data[count]&0xff;
        HwLcdSend(data1);
        HwLcdSend(data2);
    }
    TFT_CS_HIGH;
}
*/

static void LCD_Write_COM(char VL)
{
    sendCMD(VL);
}

static void LCD_Write_DATA2(char VH,char VL)
{
    WRITE_DATA(VH);
    WRITE_DATA(VL);
}

static void LCD_Write_DATA(char VL)
{
    WRITE_DATA(VL);
}


static INT8U Read_Register(INT8U Addr, INT8U xParameter)
{
    INT8U data=0;
    sendCMD(0xd9);                                                      /* ext command                  */
    WRITE_DATA(0x10+xParameter);                                        /* 0x11 is the first Parameter  */
    TFT_DC_LOW;
    TFT_CS_LOW;
    HwLcdSend(Addr);
    TFT_DC_HIGH;
    data = HwLcdSend(0);
    TFT_CS_HIGH;
    return data;
}

INT8U UTFT_readID(void)
{
    INT8U i=0;
    INT8U data[3] ;
    INT8U ID[3] = {0x00, 0x93, 0x41};
    INT8U ToF=1;
    for(i=0;i<3;i++)
    {
        data[i]=Read_Register(0xd3,i+1);
        if(data[i] != ID[i])
        {
            ToF=0;
        }
    }
    if(!ToF)                                                            /* data!=ID                     */
    {
    	//error!
    }
    return ToF;
}

void UTFT_InitLCD(byte orientation)
{

    TFT_BL_ON;
	orient=orientation;
    TFT_CS_HIGH;
    TFT_DC_HIGH;
    
	TFT_RST_ON;
	delay(50);
	TFT_RST_OFF;
/*
    for(INT8U i=0; i<3; i++)
    {
        INT8U TFTDriver = UTFT_readID();
    }
*/
    if(1)
    {
	sendCMD(0xCB);
	WRITE_DATA(0x39);
	WRITE_DATA(0x2C);
	WRITE_DATA(0x00);
	WRITE_DATA(0x34);
	WRITE_DATA(0x02);
    
	sendCMD(0xCF);
	WRITE_DATA(0x00);
	WRITE_DATA(0XC1);
	WRITE_DATA(0X30);
    
	sendCMD(0xE8);
	WRITE_DATA(0x85);
	WRITE_DATA(0x00);
	WRITE_DATA(0x78);
    
	sendCMD(0xEA);
	WRITE_DATA(0x00);
	WRITE_DATA(0x00);
    
	sendCMD(0xED);
	WRITE_DATA(0x64);
	WRITE_DATA(0x03);
	WRITE_DATA(0X12);
	WRITE_DATA(0X81);
    
	sendCMD(0xF7);
	WRITE_DATA(0x20);
    
	sendCMD(0xC0);    	//Power control
	WRITE_DATA(0x23);   	//VRH[5:0]
    
	sendCMD(0xC1);    	//Power control
	WRITE_DATA(0x10);   	//SAP[2:0];BT[3:0]
    
	sendCMD(0xC5);    	//VCM control
	WRITE_DATA(0x3e);   	//Contrast
	WRITE_DATA(0x28);
    
	sendCMD(0xC7);    	//VCM control2
	WRITE_DATA(0x86);  	 //--
    
	sendCMD(0x36);    	// Memory Access Control
	WRITE_DATA(0x48);  	//C8	   //48 68绔栧睆//28 E8 妯睆
    
	sendCMD(0x3A);
	WRITE_DATA(0x55);
    
	sendCMD(0xB1);
	WRITE_DATA(0x00);
	WRITE_DATA(0x18);
    
	sendCMD(0xB6);    	// Display Function Control
	WRITE_DATA(0x08);
	WRITE_DATA(0x82);
	WRITE_DATA(0x27);
    
	sendCMD(0xF2);    	// 3Gamma Function Disable
	WRITE_DATA(0x00);
    
	sendCMD(0x26);    	//Gamma curve selected
	WRITE_DATA(0x01);
    
	sendCMD(0xE0);    	//Set Gamma
	WRITE_DATA(0x0F);
	WRITE_DATA(0x31);
	WRITE_DATA(0x2B);
	WRITE_DATA(0x0C);
	WRITE_DATA(0x0E);
	WRITE_DATA(0x08);
	WRITE_DATA(0x4E);
	WRITE_DATA(0xF1);
	WRITE_DATA(0x37);
	WRITE_DATA(0x07);
	WRITE_DATA(0x10);
	WRITE_DATA(0x03);
	WRITE_DATA(0x0E);
	WRITE_DATA(0x09);
	WRITE_DATA(0x00);
    
	sendCMD(0XE1);    	//Set Gamma
	WRITE_DATA(0x00);
	WRITE_DATA(0x0E);
	WRITE_DATA(0x14);
	WRITE_DATA(0x03);
	WRITE_DATA(0x11);
	WRITE_DATA(0x07);
	WRITE_DATA(0x31);
	WRITE_DATA(0xC1);
	WRITE_DATA(0x48);
	WRITE_DATA(0x08);
	WRITE_DATA(0x0F);
	WRITE_DATA(0x0C);
	WRITE_DATA(0x31);
	WRITE_DATA(0x36);
	WRITE_DATA(0x0F);
    
	sendCMD(0x11);    	//Exit Sleep
	delay(120); 
    
	sendCMD(0x29);    //Display on 
	delay(120);
	sendCMD(0x2c);

	} else
	{
	 sendCMD(0x01);
	 delay(20);
	 sendCMD(0x28);

   	 sendCMD(0xCF);                     
     WRITE_DATA(0x00);
     WRITE_DATA(0x83);
     WRITE_DATA(0x30);
     
     sendCMD(0xED);                     
     WRITE_DATA(0x64);
     WRITE_DATA(0x03);
     WRITE_DATA(0x12);
     WRITE_DATA(0x81);
     
     sendCMD(0xE8);                     
     WRITE_DATA(0x85);
     WRITE_DATA(0x01);
     WRITE_DATA(0x79);
     
     sendCMD(0xCB);                     
     WRITE_DATA(0x39);
     WRITE_DATA(0x2C);
     WRITE_DATA(0x00);
     WRITE_DATA(0x34);
     WRITE_DATA(0x02);
           
     sendCMD(0xF7);                     
     WRITE_DATA(0x20);
           
     sendCMD(0xEA);                     
     WRITE_DATA(0x00);
     WRITE_DATA(0x00);
     
     sendCMD(0xC0);                     // POWER_CONTROL_1
     WRITE_DATA(0x26);
 
     sendCMD(0xC1);                     // POWER_CONTROL_2
     WRITE_DATA(0x11);
     
     sendCMD(0xC5);                     // VCOM_CONTROL_1
     WRITE_DATA(0x35);
     WRITE_DATA(0x3E);
     
     sendCMD(0xC7);                     // VCOM_CONTROL_2
     WRITE_DATA(0xBE);
     
     sendCMD(0x36);                     // MEMORY_ACCESS_CONTROL
     WRITE_DATA(0x48);
     
     sendCMD(0x3A);                     // COLMOD_PIXEL_FORMAT_SET
     WRITE_DATA(0x55);                 // 16 bit pixel 
     
     sendCMD(0xB1);                     // Frame Rate
     WRITE_DATA(0x00);
     WRITE_DATA(0x1B);               
     
     sendCMD(0xF2);                     // Gamma Function Disable
     WRITE_DATA(0x08);
     
     sendCMD(0x26);                     
     WRITE_DATA(0x01);                 // gamma set for curve 01/2/04/08
     
     sendCMD(0xE0);                     // positive gamma correction
     WRITE_DATA(0x1F); 
     WRITE_DATA(0x1A); 
     WRITE_DATA(0x18); 
     WRITE_DATA(0x0A); 
     WRITE_DATA(0x0F); 
     WRITE_DATA(0x06); 
     WRITE_DATA(0x45); 
     WRITE_DATA(0x87); 
     WRITE_DATA(0x32); 
     WRITE_DATA(0x0A); 
     WRITE_DATA(0x07); 
     WRITE_DATA(0x02); 
     WRITE_DATA(0x07);
     WRITE_DATA(0x05); 
     WRITE_DATA(0x00);
     
     sendCMD(0xE1);                     // negativ gamma correction
     WRITE_DATA(0x00); 
     WRITE_DATA(0x25); 
     WRITE_DATA(0x27); 
     WRITE_DATA(0x05); 
     WRITE_DATA(0x10); 
     WRITE_DATA(0x09); 
     WRITE_DATA(0x3A); 
     WRITE_DATA(0x78); 
     WRITE_DATA(0x4D); 
     WRITE_DATA(0x05); 
     WRITE_DATA(0x18); 
     WRITE_DATA(0x0D); 
     WRITE_DATA(0x38);
     WRITE_DATA(0x3A); 
     WRITE_DATA(0x1F);

     UTFT_clrXY();

     sendCMD(0xB7);                       // entry mode
     WRITE_DATA(0x07);
     
     sendCMD(0xB6);                       // display function control
     WRITE_DATA(0x0A);
     WRITE_DATA(0x82);
     WRITE_DATA(0x27);
     WRITE_DATA(0x00);
     
     sendCMD(0x11);                     // sleep out
     
     delay(100);
     
     sendCMD(0x29);                     // display on
     
     delay(100);
    }
    
	cfont.font=0;
	_transparent = false;
}

void UTFT_setXY(word x1, word y1, word x2, word y2)
{
	if (orient==UTFT_LANDSCAPE)
	{
		swap(word, x1, y1);
		swap(word, x2, y2)
		y1=disp_y_size-y1;
		y2=disp_y_size-y2;
		swap(word, y1, y2)
	}

	LCD_Write_COM(0x2a);
	LCD_Write_DATA(x1>>8);
	LCD_Write_DATA(x1);
	LCD_Write_DATA(x2>>8);
	LCD_Write_DATA(x2);
	LCD_Write_COM(0x2b);
	LCD_Write_DATA(y1>>8);
	LCD_Write_DATA(y1);
	LCD_Write_DATA(y2>>8);
	LCD_Write_DATA(y2);
	LCD_Write_COM(0x2c);
}

void UTFT_clrXY()
{
	if (orient==UTFT_PORTRAIT)
		UTFT_setXY(0,0,disp_x_size,disp_y_size);
	else
		UTFT_setXY(0,0,disp_y_size,disp_x_size);
}

void UTFT_drawRect(int x1, int y1, int x2, int y2)
{
	if (x1>x2)
	{
		swap(int, x1, x2);
	}
	if (y1>y2)
	{
		swap(int, y1, y2);
	}

	UTFT_drawHLine(x1, y1, x2-x1);
	UTFT_drawHLine(x1, y2, x2-x1);
	UTFT_drawVLine(x1, y1, y2-y1);
	UTFT_drawVLine(x2, y1, y2-y1);
}

void UTFT_drawRoundRect(int x1, int y1, int x2, int y2)
{
	if (x1>x2)
	{
		swap(int, x1, x2);
	}
	if (y1>y2)
	{
		swap(int, y1, y2);
	}
	if ((x2-x1)>4 && (y2-y1)>4)
	{
		UTFT_drawPixel(x1+1,y1+1);
		UTFT_drawPixel(x2-1,y1+1);
		UTFT_drawPixel(x1+1,y2-1);
		UTFT_drawPixel(x2-1,y2-1);
		UTFT_drawHLine(x1+2, y1, x2-x1-4);
		UTFT_drawHLine(x1+2, y2, x2-x1-4);
		UTFT_drawVLine(x1, y1+2, y2-y1-4);
		UTFT_drawVLine(x2, y1+2, y2-y1-4);
	}
}

void UTFT_fillRect(int x1, int y1, int x2, int y2)
{
	if (x1>x2)
	{
		swap(int, x1, x2);
	}
	if (y1>y2)
	{
		swap(int, y1, y2);
	}

		if (orient==UTFT_PORTRAIT)
		{
			for (int i=0; i<((y2-y1)/2)+1; i++)
			{
				UTFT_drawHLine(x1, y1+i, x2-x1);
				UTFT_drawHLine(x1, y2-i, x2-x1);
			}
		}
		else
		{
			for (int i=0; i<((x2-x1)/2)+1; i++)
			{
				UTFT_drawVLine(x1+i, y1, y2-y1);
				UTFT_drawVLine(x2-i, y1, y2-y1);
			}
		}
	}


void UTFT_fillRoundRect(int x1, int y1, int x2, int y2)
{
	if (x1>x2)
	{
		swap(int, x1, x2);
	}
	if (y1>y2)
	{
		swap(int, y1, y2);
	}

	if ((x2-x1)>4 && (y2-y1)>4)
	{
		for (int i=0; i<((y2-y1)/2)+1; i++)
		{
			switch(i)
			{
			case 0:
				UTFT_drawHLine(x1+2, y1+i, x2-x1-4);
				UTFT_drawHLine(x1+2, y2-i, x2-x1-4);
				break;
			case 1:
				UTFT_drawHLine(x1+1, y1+i, x2-x1-2);
				UTFT_drawHLine(x1+1, y2-i, x2-x1-2);
				break;
			default:
				UTFT_drawHLine(x1, y1+i, x2-x1);
				UTFT_drawHLine(x1, y2-i, x2-x1);
			}
		}
	}
}

void UTFT_drawCircle(int x, int y, int radius)
{
	int f = 1 - radius;
	int ddF_x = 1;
	int ddF_y = -2 * radius;
	int x1 = 0;
	int y1 = radius;
 
	//cbi(P_CS, B_CS);
	UTFT_setXY(x, y + radius, x, y + radius);
	LCD_Write_DATA2(fch,fcl);
	UTFT_setXY(x, y - radius, x, y - radius);
	LCD_Write_DATA2(fch,fcl);
	UTFT_setXY(x + radius, y, x + radius, y);
	LCD_Write_DATA2(fch,fcl);
	UTFT_setXY(x - radius, y, x - radius, y);
	LCD_Write_DATA2(fch,fcl);
 
	while(x1 < y1)
	{
		if(f >= 0) 
		{
			y1--;
			ddF_y += 2;
			f += ddF_y;
		}
		x1++;
		ddF_x += 2;
		f += ddF_x;    
		UTFT_setXY(x + x1, y + y1, x + x1, y + y1);
		LCD_Write_DATA2(fch,fcl);
		UTFT_setXY(x - x1, y + y1, x - x1, y + y1);
		LCD_Write_DATA2(fch,fcl);
		UTFT_setXY(x + x1, y - y1, x + x1, y - y1);
		LCD_Write_DATA2(fch,fcl);
		UTFT_setXY(x - x1, y - y1, x - x1, y - y1);
		LCD_Write_DATA2(fch,fcl);
		UTFT_setXY(x + y1, y + x1, x + y1, y + x1);
		LCD_Write_DATA2(fch,fcl);
		UTFT_setXY(x - y1, y + x1, x - y1, y + x1);
		LCD_Write_DATA2(fch,fcl);
		UTFT_setXY(x + y1, y - x1, x + y1, y - x1);
		LCD_Write_DATA2(fch,fcl);
		UTFT_setXY(x - y1, y - x1, x - y1, y - x1);
		LCD_Write_DATA2(fch,fcl);
	}
	//sbi(P_CS, B_CS);
	UTFT_clrXY();
}

void UTFT_fillCircle(int x, int y, int radius)
{
	for(int y1=-radius; y1<=0; y1++) 
		for(int x1=-radius; x1<=0; x1++)
			if(x1*x1+y1*y1 <= radius*radius) 
			{
				UTFT_drawHLine(x+x1, y+y1, 2*(-x1));
				UTFT_drawHLine(x+x1, y-y1, 2*(-x1));
				break;
			}
}

void UTFT_clrScr()
{
    UTFT_clrXY();
    for (int i=0; i<((disp_x_size+1)*(disp_y_size+1)); i++)
    {
        // black
        WRITE_DATA(0); // hi bit of the color
        WRITE_DATA(0); // low bit of the color
    }
}

void UTFT_fillScr(byte r, byte g, byte b)
{
	word color = ((r&248)<<8 | (g&252)<<3 | (b&248)>>3);
	UTFT_fillScrW(color);
}

void UTFT_fillScrW(word color)
{
	char ch, cl;
	
	ch=(byte)(color>>8);
	cl=(byte)(color & 0xFF);
	
    UTFT_clrXY();
    for (int i=0; i<((disp_x_size+1)*(disp_y_size+1)); i++)
    {
        // black
        WRITE_DATA(ch); // hi bit of the color
        WRITE_DATA(cl); // low bit of the color
    }
}

void UTFT_setColor(byte r, byte g, byte b)
{
	fch=((r&248)|g>>5);
	fcl=((g&28)<<3|b>>3);
}

void UTFT_setColorW(word color)
{
	fch=(byte)(color>>8);
	fcl=(byte)(color & 0xFF);
}

word UTFT_getColor()
{
	return (fch<<8) | fcl;
}

void UTFT_setBackColor(byte r, byte g, byte b)
{
	bch=((r&248)|g>>5);
	bcl=((g&28)<<3|b>>3);
	_transparent=false;
}

void UTFT_setBackColorW(uint32_t color)
{
	if (color==VGA_TRANSPARENT)
		_transparent=true;
	else
	{
		bch=(byte)(color>>8);
		bcl=(byte)(color & 0xFF);
		_transparent=false;
	}
}

word UTFT_getBackColor()
{
	return (bch<<8) | bcl;
}

void UTFT_setPixel(word color)
{
	LCD_Write_DATA2((color>>8),(color&0xFF));	// rrrrrggggggbbbbb
}

void UTFT_drawPixel(int x, int y)
{

	UTFT_setXY(x, y, x, y);
	UTFT_setPixel((fch<<8)|fcl);
	UTFT_clrXY();
}

void UTFT_drawLine(int x1, int y1, int x2, int y2)
{
	if (y1==y2)
		UTFT_drawHLine(x1, y1, x2-x1);
	else if (x1==x2)
		UTFT_drawVLine(x1, y1, y2-y1);
	else
	{
		unsigned int	dx = (x2 > x1 ? x2 - x1 : x1 - x2);
		short			xstep =  x2 > x1 ? 1 : -1;
		unsigned int	dy = (y2 > y1 ? y2 - y1 : y1 - y2);
		short			ystep =  y2 > y1 ? 1 : -1;
		int				col = x1, row = y1;

		if (dx < dy)
		{
			int t = - (dy >> 1);
			while (true)
			{
				UTFT_setXY (col, row, col, row);
				LCD_Write_DATA2 (fch, fcl);
				if (row == y2)
					return;
				row += ystep;
				t += dx;
				if (t >= 0)
				{
					col += xstep;
					t   -= dy;
				}
			} 
		}
		else
		{
			int t = - (dx >> 1);
			while (true)
			{
				UTFT_setXY (col, row, col, row);
				LCD_Write_DATA2 (fch, fcl);
				if (col == x2)
					return;
				col += xstep;
				t += dy;
				if (t >= 0)
				{
					row += ystep;
					t   -= dx;
				}
			} 
		}
	}
	UTFT_clrXY();
}

void UTFT_drawHLine(int x, int y, int l)
{
	if (l<0)
	{
		l = -l;
		x -= l;
	}
	UTFT_setXY(x, y, x+l, y);
	
	for (int i=0; i<l+1; i++)
	{
		LCD_Write_DATA2(fch, fcl);
	}
	

	UTFT_clrXY();
}

void UTFT_drawVLine(int x, int y, int l)
{
	if (l<0)
	{
		l = -l;
		y -= l;
	}

	UTFT_setXY(x, y, x, y+l);

	for (int i=0; i<l+1; i++)
	{
		LCD_Write_DATA2(fch, fcl);
	}

	UTFT_clrXY();
}

void UTFT_printChar(byte c, int x, int y)
{
	byte i,ch;
	word j;
	word temp; 


  
	if (!_transparent)
	{
		if (orient==UTFT_PORTRAIT)
		{
			UTFT_setXY(x,y,x+cfont.x_size-1,y+cfont.y_size-1);
	  
			temp=((c-cfont.offset)*((cfont.x_size/8)*cfont.y_size))+4;
			for(j=0;j<((cfont.x_size/8)*cfont.y_size);j++)
			{
				ch=fontbyte(temp);
				for(i=0;i<8;i++)
				{   
					if((ch&(1<<(7-i)))!=0)   
					{
						UTFT_setPixel((fch<<8)|fcl);
					} 
					else
					{
						UTFT_setPixel((bch<<8)|bcl);
					}   
				}
				temp++;
			}
		}
		else
		{
			temp=((c-cfont.offset)*((cfont.x_size/8)*cfont.y_size))+4;

			for(j=0;j<((cfont.x_size/8)*cfont.y_size);j+=(cfont.x_size/8))
			{
				UTFT_setXY(x,y+(j/(cfont.x_size/8)),x+cfont.x_size-1,y+(j/(cfont.x_size/8)));
				for (int zz=(cfont.x_size/8)-1; zz>=0; zz--)
				{
					ch=fontbyte(temp+zz);
					for(i=0;i<8;i++)
					{   
						if((ch&(1<<i))!=0)   
						{
							UTFT_setPixel((fch<<8)|fcl);
						} 
						else
						{
							UTFT_setPixel((bch<<8)|bcl);
						}   
					}
				}
				temp+=(cfont.x_size/8);
			}
		}
	}
	else
	{
		temp=((c-cfont.offset)*((cfont.x_size/8)*cfont.y_size))+4;
		for(j=0;j<cfont.y_size;j++) 
		{
			for (int zz=0; zz<(cfont.x_size/8); zz++)
			{
				ch=fontbyte(temp+zz); 
				for(i=0;i<8;i++)
				{   
					UTFT_setXY(x+i+(zz*8),y+j,x+i+(zz*8)+1,y+j+1);
				
					if((ch&(1<<(7-i)))!=0)   
					{
						UTFT_setPixel((fch<<8)|fcl);
					} 
				}
			}
			temp+=(cfont.x_size/8);
		}
	}


	UTFT_clrXY();
}

void UTFT_rotateChar(byte c, int x, int y, int pos, int deg)
{
	byte i,j,ch;
	word temp; 
	int newx,newy;
	double radian;
	radian=deg*0.0175;  


	temp=((c-cfont.offset)*((cfont.x_size/8)*cfont.y_size))+4;
	for(j=0;j<cfont.y_size;j++) 
	{
		for (int zz=0; zz<(cfont.x_size/8); zz++)
		{
			ch=fontbyte(temp+zz);
			for(i=0;i<8;i++)
			{   
				newx=x+(((i+(zz*8)+(pos*cfont.x_size))*cos(radian))-((j)*sin(radian)));
				newy=y+(((j)*cos(radian))+((i+(zz*8)+(pos*cfont.x_size))*sin(radian)));

				UTFT_setXY(newx,newy,newx+1,newy+1);
				
				if((ch&(1<<(7-i)))!=0)   
				{
					UTFT_setPixel((fch<<8)|fcl);
				} 
				else  
				{
					if (!_transparent)
						UTFT_setPixel((bch<<8)|bcl);
				}   
			}
		}
		temp+=(cfont.x_size/8);
	}

	UTFT_clrXY();
}

void UTFT_print(char *st, int x, int y, int deg)
{
	int stl, i;

	stl = strlen(st);

	if (orient==UTFT_PORTRAIT)
	{
	if (x==UTFT_RIGHT)
		x=(disp_x_size+1)-(stl*cfont.x_size);
	if (x==UTFT_CENTER)
		x=((disp_x_size+1)-(stl*cfont.x_size))/2;
	}
	else
	{
	if (x==UTFT_RIGHT)
		x=(disp_y_size+1)-(stl*cfont.x_size);
	if (x==UTFT_CENTER)
		x=((disp_y_size+1)-(stl*cfont.x_size))/2;
	}

	for (i=0; i<stl; i++)
		if (deg==0)
			UTFT_printChar(*st++, x + (i*(cfont.x_size)), y);
		else
			UTFT_rotateChar(*st++, x, y, i, deg);
}

void UTFT_printNumI(long num, int x, int y, int length, char filler)
{
	char buf[25];
	char st[27];
	bool neg=false;
	int c=0, f=0;
  
	if (num==0)
	{
		if (length!=0)
		{
			for (c=0; c<(length-1); c++)
				st[c]=filler;
			st[c]=48;
			st[c+1]=0;
		}
		else
		{
			st[0]=48;
			st[1]=0;
		}
	}
	else
	{
		if (num<0)
		{
			neg=true;
			num=-num;
		}
	  
		while (num>0)
		{
			buf[c]=48+(num % 10);
			c++;
			num=(num-(num % 10))/10;
		}
		buf[c]=0;
	  
		if (neg)
		{
			st[0]=45;
		}
	  
		if (length>(c+neg))
		{
			for (int i=0; i<(length-c-neg); i++)
			{
				st[i+neg]=filler;
				f++;
			}
		}

		for (int i=0; i<c; i++)
		{
			st[i+neg+f]=buf[c-i-1];
		}
		st[c+neg+f]=0;

	}

	UTFT_print(st,x,y,0);
}
/*
void UTFT_printNumF(double num, byte dec, int x, int y, char divider, int length, char filler)
{
	char st[27];
	bool neg=false;

	if (dec<1)
		dec=1;
	else if (dec>5)
		dec=5;

	if (num<0)
		neg = true;

	UTFT_convert_float(st, num, length, dec);

	if (divider != '.')
	{
		for (int i=0; i<sizeof(st); i++)
			if (st[i]=='.')
				st[i]=divider;
	}

	if (filler != ' ')
	{
		if (neg)
		{
			st[0]='-';
			for (int i=1; i<sizeof(st); i++)
				if ((st[i]==' ') || (st[i]=='-'))
					st[i]=filler;
		}
		else
		{
			for (int i=0; i<sizeof(st); i++)
				if (st[i]==' ')
					st[i]=filler;
		}
	}

	UTFT_print(st,x,y,0);
}
*/
void UTFT_setFont(uint8_t* font)
{
	cfont.font=font;
	cfont.x_size=fontbyte(0);
	cfont.y_size=fontbyte(1);
	cfont.offset=fontbyte(2);
	cfont.numchars=fontbyte(3);
}

uint8_t* UTFT_getFont()
{
	return cfont.font;
}

uint8_t UTFT_getFontXsize()
{
	return cfont.x_size;
}

uint8_t UTFT_getFontYsize()
{
	return cfont.y_size;
}

void UTFT_drawBitmap(int x, int y, int sx, int sy, bitmapdatatype data, int scale)
{
	unsigned int col;
	int tx, ty, tc, tsx, tsy;

	if (scale==1)
	{
		if (orient==UTFT_PORTRAIT)
		{

			UTFT_setXY(x, y, x+sx-1, y+sy-1);
			for (tc=0; tc<(sx*sy); tc++)
			{
				col=data[tc];
				LCD_Write_DATA2(col>>8,col & 0xff);
			}
		}
		else
		{
			for (ty=0; ty<sy; ty++)
			{
				UTFT_setXY(x, y+ty, x+sx-1, y+ty);
				for (tx=sx-1; tx>=0; tx--)
				{
					col=data[(ty*sx)+tx];
					LCD_Write_DATA2(col>>8,col & 0xff);
				}
			}

		}
	}
	else
	{
		if (orient==UTFT_PORTRAIT)
		{

			for (ty=0; ty<sy; ty++)
			{
				UTFT_setXY(x, y+(ty*scale), x+((sx*scale)-1), y+(ty*scale)+scale);
				for (tsy=0; tsy<scale; tsy++)
					for (tx=0; tx<sx; tx++)
					{
						col=data[(ty*sx)+tx];
						for (tsx=0; tsx<scale; tsx++)
							LCD_Write_DATA2(col>>8,col & 0xff);
					}
			}

		}
		else
		{

			for (ty=0; ty<sy; ty++)
			{
				for (tsy=0; tsy<scale; tsy++)
				{
					UTFT_setXY(x, y+(ty*scale)+tsy, x+((sx*scale)-1), y+(ty*scale)+tsy);
					for (tx=sx-1; tx>=0; tx--)
					{
						col=data[(ty*sx)+tx];
						for (tsx=0; tsx<scale; tsx++)
							LCD_Write_DATA2(col>>8,col & 0xff);
					}
				}
			}

		}
	}
	UTFT_clrXY();
}

void UTFT_drawBitmapR(int x, int y, int sx, int sy, bitmapdatatype data, int deg, int rox, int roy)
{
	unsigned int col;
	int tx, ty, newx, newy;
	double radian;
	radian=deg*0.0175;  

	if (deg==0)
		UTFT_drawBitmap(x, y, sx, sy, data, 1);
	else
	{
			for (ty=0; ty<sy; ty++)
			for (tx=0; tx<sx; tx++)
			{
				col=data[(ty*sx)+tx];

				newx=x+rox+(((tx-rox)*cos(radian))-((ty-roy)*sin(radian)));
				newy=y+roy+(((ty-roy)*cos(radian))+((tx-rox)*sin(radian)));

				UTFT_setXY(newx, newy, newx, newy);
				LCD_Write_DATA2(col>>8,col & 0xff);
			}
	}
	UTFT_clrXY();
}

void UTFT_lcdOff()
{

}

void UTFT_lcdOn()
{

}

void UTFT_setContrast(char c)
{

}

int UTFT_getDisplayXSize()
{
	if (orient==UTFT_PORTRAIT)
		return disp_x_size+1;
	else
		return disp_y_size+1;
}

int UTFT_getDisplayYSize()
{
	if (orient==UTFT_PORTRAIT)
		return disp_y_size+1;
	else
		return disp_x_size+1;
}
