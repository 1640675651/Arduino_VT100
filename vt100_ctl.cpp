#include "vt100_ctl.h"
uint16_t term_colors[8] = {BLACK_16, RED_16, GREEN_16, YELLOW_16, BLUE_16, VIOLET_16, CYAN_16, WHITE_16};
extern uint16_t bg_color;

//TODO: scrolling
char ctlcode[20];

bool isletter(char c)
{
  if((c>='A' && c<='Z') || (c>='a' && c<='z'))
    return true;
  return false;
}

uint8_t read_ctl_code(char* ctlcode)
{
  uint8_t ctlcode_len = 0;
  Serial.readBytes(ctlcode, 1);
  while(!isletter(ctlcode[ctlcode_len]))
  {
    ctlcode_len++;
    Serial.readBytes(&ctlcode[ctlcode_len], 1);
  }
  ctlcode_len++;
  ctlcode[ctlcode_len] = 0;
  return ctlcode_len;
}
/*
void extract_param(char* ctlcode, uint8_t ctlcode_len, int8_t& param0, int8_t& param1)
{
  switch(ctlcode_len)
  {
    case 1: // no parameter
      break;
    case 2: // 1 parameter
      param0 = ctlcode[0] - '0';
      break;
    case 4: // 2 parameters
      param0 = ctlcode[0] - '0';
      param1 = ctlcode[2] - '0';
  }
}
*/
void extract_param(char* ctlcode, uint8_t ctlcode_len, int8_t& param0, int8_t& param1)
{
  bool two_param = false;
  for(uint8_t i=0;i<ctlcode_len-1;i++)
  {
    if(ctlcode[i] == ';')
    {
      two_param = true;
      continue;
    }
    if(!two_param)
    {
      param0 *= 10;
      param0 += ctlcode[i] - '0';
    }
    else
    {
      param1 *= 10;
      param1 += ctlcode[i] - '0';
    }
  }
}


void parse_single_m_code(char* ctlcode, uint8_t cmdstart, uint8_t cmdend, Adafruit_TFTLCD &tft)
{
  if(cmdstart == cmdend) // empty command
    return;
    
  while(ctlcode[cmdstart] == '0')
    cmdstart++;

  if(cmdstart == cmdend) // characters in the command are all '0'
    reset_settings(tft);
  else if(cmdend - cmdstart == 1) // 1-digit command
    switch(ctlcode[cmdstart]) //not implemented
    {
      case 1: //bold
        break;
      case 2: //dim
        break;
      case 4: //underline
        break;
      case 5: //blink
        break;
      case 7: //reverse video mode
        break;
      case 8: //invisible text mode
        break;
    }
  else if(cmdend - cmdstart == 2) //2-digit command
  {
    if(ctlcode[cmdstart] == '3') //set text color
      terminalSetTextColor(ctlcode[cmdstart+1], tft);
    else if(ctlcode[cmdstart] == '4') //set background color
      terminalSetBackgroundColor(ctlcode[cmdstart+1], tft);
  }
  //else command of invalid length
  
}

//a control code ending with m can contain multiple sub-codes, separated by ';'.
void parse_m_code(char* ctlcode, uint8_t ctlcode_len, Adafruit_TFTLCD &tft)
{
  uint8_t i = 0;
  uint8_t lasti = 0;
  while(i<ctlcode_len)
  {
    while(ctlcode[i] != ';' && ctlcode[i] != 'm')
    {
      i++;
    }
    parse_single_m_code(ctlcode, lasti, i, tft); //[lasti, i)
    i++;
    lasti = i;
  }
}

void parse_ctl_code(Adafruit_TFTLCD &tft)
{
  uint8_t ctlcode_len = read_ctl_code(ctlcode);
  char ctl_type = ctlcode[ctlcode_len - 1];
  if(ctl_type != 'm')
  {
    int8_t param0 = 0;
    int8_t param1 = 0;
    extract_param(ctlcode, ctlcode_len, param0, param1);
    int8_t cursor_delta=(param0?param0:1);
    switch (ctl_type)
    {
      case 'A': //up
        moveCursor(0, -cursor_delta, tft);
        break;
      case 'B': //down
        moveCursor(0, cursor_delta, tft);
        break;
      case 'C': //right
        moveCursor(cursor_delta, 0, tft);
        break;
      case 'D': //left
        moveCursor(-cursor_delta, 0, tft);
        break;
      case 'G': //It seems that this big 'G' is used to set column
        if(param0)
          setCursorX(param0-1, tft);
        else
          setCursorX(0, tft);
        break;
      case 'H': //reset cursor
        if(param0 == 0 || param1 == 0)
          tft.setCursor(0, 0);
        else
          setCursorwithXY(param1-1 ,param0-1, tft);
        break;
      case 'J': //clear screen
        if(param0 == 1)
          clearScreenbeforeCursor(tft);
        else if(param0 == 2) //clear entire screen
          tft.fillScreen(bg_color);
        else
          clearScreenafterCursor(tft);
        break;
      case 'K': //delete everything on the right/left/line of the cursor
        if(param0 == 1)
          delCursorLeft(tft);
        else if(param0 == 2) //delete entire line
          delCursorLine(tft);
        else
          delCursorRight(tft);
        break;
      case 'd': //it seems this little 'd' is used to set row
        if(param0)
          setCursorY(param0-1, tft);
        else
          setCursorY(0, tft);
        break;
    }
  }
  else
  {
    parse_m_code(ctlcode, ctlcode_len, tft);
  }
}

void reset_settings(Adafruit_TFTLCD &tft)
{
  tft.setTextColor(DEFAULT_TEXT_COLOR);
  bg_color = DEFAULT_BG_COLOR;
}

void terminalSetTextColor(char color, Adafruit_TFTLCD &tft)
{
  if(color == '9')
    tft.setTextColor(DEFAULT_TEXT_COLOR);
  else
    tft.setTextColor(term_colors[color - '0']);
}

void terminalSetBackgroundColor(char color, Adafruit_TFTLCD &tft)
{
  if(color == '9')
    bg_color = DEFAULT_BG_COLOR;
  else
    bg_color = term_colors[color - '0'];
}

//note: vt100 line and column number start from 1, but our screen space start from 0.
