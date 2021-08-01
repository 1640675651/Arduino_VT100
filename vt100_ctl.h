#ifndef VT100_CTL_H
#define VT100_CTL_H

#include "lcd.h"

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

void parse_ctl_code(Adafruit_TFTLCD &tft);

void reset_settings(Adafruit_TFTLCD &tft);

void terminalSetTextColor(char color, Adafruit_TFTLCD &tft);

void terminalSetBackgroundColor(char color, Adafruit_TFTLCD &tft);

#endif
