#ifndef LCD_H
#define LCD_H

#include <Adafruit_GFX.h>
#include <Adafruit_TFTLCD.h>
#include <pin_magic.h>
#include "256color.h"

#define LCD_RD   A0
#define LCD_WR   A1     
#define LCD_CD   A2        
#define LCD_CS   A3       
#define LCD_RESET A4

//#define PORTRAIT
#ifdef PORTRAIT
  #define W_PIXEL 240
  #define H_PIXEL 320
  #define W_CHARS 40
  #define H_CHARS 40
  #define ROTATION 0
#else
  #define W_PIXEL 320
  #define H_PIXEL 240
  #define W_CHARS 53
  #define H_CHARS 30
  #define ROTATION 1
#endif

#define CHAR_WIDTH 6
#define CHAR_HEIGHT 8

#define BLACK_16 0
#define RED_16 0b1111100000000000
#define GREEN_16 0b0000011111100000
#define YELLOW_16 0b1111111111100000
#define BLUE_16 0b0000000000011111
#define VIOLET_16 0b1111100000011111
#define CYAN_16 0b0000011111111111
#define WHITE_16 0xFFFF
#define DEFAULT_TEXT_COLOR WHITE_16
#define DEFAULT_BG_COLOR BLACK_16

void lcdinit(Adafruit_TFTLCD &tft);

void getCursorfromLCD(uint8_t &x, uint8_t &y, Adafruit_TFTLCD &tft);
void setCursorwithXY(uint8_t x, uint8_t y, Adafruit_TFTLCD &tft);
void setCursorX(uint8_t x, Adafruit_TFTLCD &tft);
void setCursorY(uint8_t y, Adafruit_TFTLCD &tft);
void moveCursor(int8_t dx, int8_t dy, Adafruit_TFTLCD &tft);
void delCursorRight(Adafruit_TFTLCD &tft);
void delCursorLeft(Adafruit_TFTLCD &tft);
void delCursorLine(Adafruit_TFTLCD &tft);
void overWrite(char c, Adafruit_TFTLCD &tft);
void clearScreenbeforeCursor(Adafruit_TFTLCD &tft);
void clearScreenafterCursor(Adafruit_TFTLCD &tft);
void moveCursorandScroll(int8_t dy, Adafruit_TFTLCD &tft);
void scrollUp(uint8_t d, Adafruit_TFTLCD &tft);
void scrollDown(uint8_t d, Adafruit_TFTLCD &tft);
void copyLineBlock(uint8_t srcY, uint8_t tgtY, Adafruit_TFTLCD &tft);
void copyLine(uint16_t srcy, uint16_t tgty, Adafruit_TFTLCD &tft);
void readLineFast(uint16_t y, Adafruit_TFTLCD &tft);
void setScrollArea(uint16_t tfa, uint16_t vsa, uint16_t bfa);
void scrollNative(uint16_t vsp);
void scrollUpNative(uint8_t d, Adafruit_TFTLCD &tft);
void scrollDownNative(uint8_t d, Adafruit_TFTLCD &tft);

#endif
