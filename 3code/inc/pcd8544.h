#ifndef _PCD8544_H_
#define _PCD8544_H_

#include <stdint.h>

/* For return value */
#define OK				0
#define OUT_OF_BORDER	1
#define OK_WITH_WRAP	2


#define LCD_X_RES			84    /* x resolution */
#define LCD_Y_RES			48    /* y resolution */
#define EMPTY_SPACE_BARS	2
#define BAR_X				5
#define BAR_Y				38

/* Cache size in bytes ( 84 * 48 ) / 8 = 504 bytes */
#define LCD_CACHE_SIZE		( ( LCD_X_RES * LCD_Y_RES ) / 8)

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
void LcdInit( void );

void LcdClear( void );
void LcdUpdate( void );
void LcdImage( const uint8_t *imageData );
void LcdContrast( uint8_t contrast);
uint8_t LcdGotoXYFont( uint8_t x, uint8_t y );
uint8_t LcdChr( LcdFontSize size, char ch );
uint8_t LcdStr( LcdFontSize size, const char* dataArray);
uint8_t LcdPixel( uint8_t x, uint8_t y, LcdPixelMode mode );
uint8_t LcdLine( uint8_t x1, uint8_t x2, uint8_t y1, uint8_t y2, LcdPixelMode mode );
uint8_t LcdRect( uint8_t x1, uint8_t x2, uint8_t y1, uint8_t y2, LcdPixelMode mode );
uint8_t LcdSingleBar( uint8_t baseX, uint8_t baseY, uint8_t height, uint8_t width, LcdPixelMode mode );
uint8_t LcdBars( uint8_t data[], uint8_t numbBars, uint8_t width, uint8_t multiplier );
void LcdDrawBattery( int32_t value );

#endif  /*  _PCD8544_H_ */
