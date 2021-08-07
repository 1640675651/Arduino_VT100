#include "lcd.h"
#include "lcd_accel.h"

uint16_t bg_color = DEFAULT_BG_COLOR;
uint8_t scroll_top = 0;
uint8_t scroll_bottom = H_CHARS - 1;
uint16_t lineData[W_PIXEL];

volatile uint8_t *csPort, *cdPort, *wrPort, *rdPort;

uint8_t csPinSet;
uint8_t cdPinSet;
uint8_t wrPinSet;
uint8_t rdPinSet;
uint8_t csPinUnset;
uint8_t cdPinUnset;
uint8_t wrPinUnset;
uint8_t rdPinUnset;

void lcdinit(Adafruit_TFTLCD &tft)
{
  tft.begin(0x9341); //begin方法根据型号选择驱动
                     //如果不加参数，默认型号为0x9325
  tft.setRotation(3); //设置屏幕方向   
  tft.fillScreen(0x0);
  tft.setCursor(0,0); //设置光标位置，每个英文字符高8宽6
                       //Adafruit_TFTLCD类继承自Adafruit_GFX，而Adafruit_GFX类继承自Print
                       //setCursor方法应该属于Print类
  //these are copied from Adafruit_TFTLCD.cpp
  csPort     = portOutputRegister(digitalPinToPort(LCD_CS));
  cdPort     = portOutputRegister(digitalPinToPort(LCD_CD));
  wrPort     = portOutputRegister(digitalPinToPort(LCD_WR));
  rdPort     = portOutputRegister(digitalPinToPort(LCD_RD));
  csPinSet = digitalPinToBitMask(LCD_CS);
  cdPinSet = digitalPinToBitMask(LCD_CD);
  wrPinSet = digitalPinToBitMask(LCD_WR);
  rdPinSet = digitalPinToBitMask(LCD_RD);
  csPinUnset = ~csPinSet;
  cdPinUnset = ~cdPinSet;
  wrPinUnset = ~wrPinSet;
  rdPinUnset = ~rdPinSet;
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

void setCursorX(uint8_t x, Adafruit_TFTLCD &tft)
{
  uint16_t cur_y = tft.getCursorY();
  tft.setCursor((uint16_t)x*6, cur_y);
}

void setCursorY(uint8_t y, Adafruit_TFTLCD &tft)
{
  uint16_t cur_x = tft.getCursorX();
  tft.setCursor(cur_x, y<<3);
}

void moveCursor(int8_t dX, int8_t dY, Adafruit_TFTLCD &tft)
{
  uint8_t X, Y;
  getCursorfromLCD(X, Y, tft);
  //X += dX; this is weird, but dX is signed, X is unsigned.
  dX += X; // if we use X += dX, when X+dX < 0 it will give unexpected behavior
  if(dX<0)
    dX = 0;
  else if(dX>=W_CHARS)
    dX = W_CHARS - 1;
  //Y += dY; same as above
  dY += Y;
  if(dY<0)
    dY = 0;
  else if(dY>=H_CHARS)
    dY = H_CHARS - 1;
  setCursorwithXY(dX, dY, tft);
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
  if(c == '\n') 
  {
    uint8_t X, Y;
    getCursorfromLCD(X, Y, tft);
    if(Y == scroll_bottom) //scroll if cursor at bottom
    {
      scrollUp(1, tft);
      moveCursor(0, -1, tft);
    }
    tft.print('\n'); 
  }
  else if(c == '\r')//in adafruit_gfx, printing '\r' does not return the cursor to the leftmost position
    setCursorX(0, tft);
  else //normal character, first erase the existing character
  {
    tft.fillRect(tft.getCursorX(), tft.getCursorY(), CHAR_WIDTH, CHAR_HEIGHT, bg_color);
    tft.print(c);
  }
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

void moveCursorandScroll(int8_t dY, Adafruit_TFTLCD &tft)
{
  uint8_t X, Y;
  getCursorfromLCD(X, Y, tft);
  if(Y >= scroll_top && Y <= scroll_bottom) //only trigger scrolling when current cursor is within scroll range
  {
    int8_t endY = Y+dY;
    if(endY > scroll_bottom) // scroll up
    {
      scrollUp(endY - scroll_bottom, tft);
    }
    else if(endY < scroll_top) //scroll down
    {
      scrollDown(-endY, tft);
    }
  }
  moveCursor(0, dY, tft);
}

void scrollUp(uint8_t d, Adafruit_TFTLCD &tft)
{
  for(uint8_t i=scroll_top; i<=scroll_bottom-d; i++)
  {
    copyLineBlock(i+d, i, tft);
  }
  uint16_t y = (scroll_bottom-d+1) << 3;
  tft.fillRect(0, y, W_PIXEL, d<<3, bg_color);
}

void scrollDown(uint8_t d, Adafruit_TFTLCD &tft)
{
  for(uint8_t i=scroll_bottom; i>=scroll_top+d; i--)
  {
    copyLineBlock(i-d, i, tft);
  }
  uint16_t y = scroll_top << 3;
  tft.fillRect(0, y, W_PIXEL, d<<3, bg_color);
}

void copyLineBlock(uint8_t srcY, uint8_t tgtY, Adafruit_TFTLCD &tft)
{
  uint8_t srcy = srcY<<3; //255 > 240 pixel in height
  uint8_t tgty = tgtY<<3;
  for(uint8_t i=0;i<CHAR_HEIGHT;i++)
  {
    copyLine(srcy+i, tgty+i, tft);
  }
}

void copyLine(uint8_t srcy, uint8_t tgty, Adafruit_TFTLCD &tft)
{
  readLineFast(srcy, tft);
  tft.setAddrWindow(0, tgty, W_PIXEL, tgty);
  tft.pushColors(lineData, W_PIXEL, true); //modified the second parameter of pushColors
  /*for(int i=0;i<W_PIXEL;i++)
  {
    tft.drawPixel(i, tgty, tft.readPixel(i, srcy));
  }*/
}


/*void readLineFast(uint8_t y, Adafruit_TFTLCD &tft)
{
  tft.setAddrWindow(0,y,W_PIXEL-1,y);
  uint8_t r=0;
  uint8_t g=0;
  uint8_t b=0;
  CS_ACTIVE;
  CD_COMMAND;
  write8(0x2e);
  setReadDirInline();
  CD_DATA;
  read8inline(r); //dummy read
  for(int i=0;i<W_PIXEL;i++)
  {
    read8inline(r);
    read8inline(g);
    read8inline(b);
    lineData[i] = tft.color565(r,g,b);
  }
  setWriteDirInline();
  CS_IDLE;
}*/
void readLineFast(uint8_t y, Adafruit_TFTLCD &tft)
{
  tft.setAddrWindow(0,y,W_PIXEL-1,y);
  uint8_t color8;
  uint16_t color16=0;
  CS_ACTIVE;
  CD_COMMAND;
  write8(0x2e);
  setReadDirInline();
  CD_DATA;
  read8inline(color8); //dummy read
  read8inline(color8); //first red5
  readG(color8, color16);
  readB(color8, color16);
  for(int i=0;i<W_PIXEL-1;i++)
  {
    readR(color8, lineData, color16, i);
    readG(color8, color16);
    readB(color8, color16);
  }
  lineData[W_PIXEL-1] = color16|(color8>>3);
  setWriteDirInline();
  CS_IDLE;
}
