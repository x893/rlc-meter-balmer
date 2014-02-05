/*
 *
 * Name         :  pcd8544.h
 *
 * Description  :  This is header file for the PCD8544 graphic LCD driver.
 *                 Based on the code written by Sylvain Bissonette
 *
 * Author       :  Fandi Gunawan <fandigunawan@gmail.com>
 * Website      :  http://fandigunawan.wordpress.com
 *
 * Credit       :  Sylvain Bissonette (2003)
 *                 Louis Frigon (2003)
 *
 * License      :  GPL v. 3
 *
 * Compiler     :  WinAVR, GCC for AVR platform
 *                 Tested version :
 *                 - 20070525
 *                 - 20071221
 *                 - 20081225
 * Compiler note:  Please be aware of using older/newer version since WinAVR
 *                 is in extensive development. Please compile with parameter -O1
 *
 * History      :
 * Please refer to pcd8544.c
 */

#ifndef _PCD8544_H_
#define _PCD8544_H_

/* For return value */
#define OK                         0
#define OUT_OF_BORDER              1
#define OK_WITH_WRAP               2


#define LCD_X_RES                  84    /* x resolution */
#define LCD_Y_RES                  48    /* y resolution */
#define EMPTY_SPACE_BARS           2
#define BAR_X                      5
#define BAR_Y                      38

/* Cache size in bytes ( 84 * 48 ) / 8 = 504 bytes */
#define LCD_CACHE_SIZE             ( ( LCD_X_RES * LCD_Y_RES ) / 8)

/* Type definition */
//typedef char                       bool;
typedef unsigned char              byte;
typedef unsigned int               word;

/* Enumeration */

typedef enum
{
    PIXEL_OFF =  0,
    PIXEL_ON  =  1,
    PIXEL_XOR =  2

} LcdPixelMode;

typedef enum
{
    FONT_1X = 1,
    FONT_2X = 2

} LcdFontSize;

/* Function prototypes */
void LcdInit       ( void );

void LcdClear      ( void );
void LcdUpdate     ( void );
void LcdImage      ( const byte *imageData );
void LcdContrast   ( byte contrast);
byte LcdGotoXYFont ( byte x, byte y );
byte LcdChr        ( LcdFontSize size, char ch );
byte LcdStr        ( LcdFontSize size, char dataArray[] );
byte LcdPixel      ( byte x, byte y, LcdPixelMode mode );
byte LcdLine       ( byte x1, byte x2, byte y1, byte y2, LcdPixelMode mode );
byte LcdRect       ( byte x1, byte x2, byte y1, byte y2, LcdPixelMode mode );
byte LcdSingleBar  ( byte baseX, byte baseY, byte height, byte width, LcdPixelMode mode );
byte LcdBars       ( byte data[], byte numbBars, byte width, byte multiplier );


#endif  /*  _PCD8544_H_ */
