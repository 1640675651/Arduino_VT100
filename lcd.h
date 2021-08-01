#ifndef LCD_H
#define LCD_H

#include <Adafruit_GFX.h>
#include <Adafruit_TFTLCD.h>

#define LCD_RD   A0
#define LCD_WR   A1     
#define LCD_CD   A2        
#define LCD_CS   A3       
#define LCD_RESET A4

#define W_PIXEL 320
#define H_PIXEL 240
#define CHAR_WIDTH 6
#define CHAR_HEIGHT 8

void lcdinit(Adafruit_TFTLCD &tft);

void getCursorfromLCD(uint8_t &x, uint8_t &y, Adafruit_TFTLCD &tft);
void setCursorwithXY(uint8_t x, uint8_t y, Adafruit_TFTLCD &tft);
void moveCursor(int8_t dx, int8_t dy, Adafruit_TFTLCD &tft);
void delCursorRight(Adafruit_TFTLCD &tft);
void delCursorLeft(Adafruit_TFTLCD &tft);
void delCursorLine(Adafruit_TFTLCD &tft);
void overWrite(char c, Adafruit_TFTLCD &tft);
void clearScreenbeforeCursor(Adafruit_TFTLCD &tft);
void clearScreenafterCursor(Adafruit_TFTLCD &tft);

#endif
