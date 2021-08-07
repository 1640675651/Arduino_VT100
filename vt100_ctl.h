#ifndef VT100_CTL_H
#define VT100_CTL_H

#include "lcd.h"

void parse_ctl_code(Adafruit_TFTLCD &tft);

void reset_settings(Adafruit_TFTLCD &tft);

void terminalSetTextColor(char color, Adafruit_TFTLCD &tft);

void terminalSetBackgroundColor(char color, Adafruit_TFTLCD &tft);

#endif
