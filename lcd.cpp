#include "lcd.h"

uint16_t bg_color = 0; // BLACK

void lcdinit(Adafruit_TFTLCD &tft)
{
  tft.begin(0x9341); //begin方法根据型号选择驱动
                     //如果不加参数，默认型号为0x9325
  tft.setRotation(3); //设置屏幕方向   
  tft.fillScreen(0x0);
  tft.setCursor(0,0); //设置光标位置，每个英文字符高8宽6
                       //Adafruit_TFTLCD类继承自Adafruit_GFX，而Adafruit_GFX类继承自Print
                       //setCursor方法应该属于Print类
}

void getCursorfromLCD(uint8_t &x, uint8_t &y, Adafruit_TFTLCD &tft)
{
  x = tft.getCursorX() / 6;
  y = tft.getCursorY() >> 3;
}

void setCursorwithXY(uint8_t x, uint8_t y, Adafruit_TFTLCD &tft)
{
  tft.setCursor((uint16_t)x*6, (uint16_t)y<<3);  
}

void moveCursor(int8_t dx, int8_t dy, Adafruit_TFTLCD &tft)
{
  uint8_t x,y;
  getCursorfromLCD(x, y, tft);
  x += dx;
  y += dy;
  setCursorwithXY(x, y, tft);
}

void delCursorRight(Adafruit_TFTLCD &tft)
{
  uint16_t x = tft.getCursorX();
  uint16_t y = tft.getCursorY();
  tft.fillRect(x, y, W_PIXEL-x, CHAR_HEIGHT, bg_color);
}

void delCursorLeft(Adafruit_TFTLCD &tft)
{
  uint16_t x = tft.getCursorX();
  uint16_t y = tft.getCursorY();
  tft.fillRect(0, y, x, CHAR_HEIGHT, bg_color);
}

void delCursorLine(Adafruit_TFTLCD &tft)
{
  uint16_t x = tft.getCursorX();
  uint16_t y = tft.getCursorY();
  tft.fillRect(0, y, W_PIXEL, CHAR_HEIGHT, bg_color);
}

void overWrite(char c, Adafruit_TFTLCD &tft)
{
  if(c!='\n' && c!='\r')//if print a newline character, don't delete the charcter at the cursor
    tft.fillRect(tft.getCursorX(), tft.getCursorY(), CHAR_WIDTH, CHAR_HEIGHT, bg_color);
  tft.print(c);
}

void clearScreenbeforeCursor(Adafruit_TFTLCD &tft)
{
  uint16_t x = tft.getCursorX();
  uint16_t y = tft.getCursorY();
  tft.fillRect(0, 0, W_PIXEL, y, bg_color);
  delCursorLeft(tft);
}

void clearScreenafterCursor(Adafruit_TFTLCD &tft)
{
  uint16_t x = tft.getCursorX();
  uint16_t y = tft.getCursorY() + CHAR_HEIGHT;
  delCursorRight(tft);
  tft.fillRect(0, y, W_PIXEL, H_PIXEL - y, bg_color);
}
