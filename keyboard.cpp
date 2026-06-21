#include "keyboard.h"

extern bool xoff_enabled;
inline void set_xoff()
{
  xoff_enabled = true;
  Serial.write(0x13);
}

inline void set_xon()
{
  xoff_enabled = false;
  Serial.write(0x11);  
}

void KbdRptParser::OnKeyDown(uint8_t mod, uint8_t key)
{
  MODIFIERKEYS Mod;
  *((uint8_t*)&Mod) = mod;
  if(Mod.bmLeftCtrl == 1)
  {
    switch(key)
    {
      case 0x6: //c
        Serial.write(0x03); // ^C
        break;
      case 0x14: //q
        set_xon();
        break;
      case 0x16: //s
        set_xoff();
        break;
    }
  }
  else
  {
    switch(key)
    {
      case 40: //enter
        Serial.print('\n');
        break;
      case 42: //backspace
        Serial.write(8);
        break;
      case 79: //right
        Serial.write('\e');
        Serial.write('[');
        Serial.write('C');
        //set_xoff();
        break;
      case 80: //left
        Serial.write('\e');
        Serial.write('[');
        Serial.write('D');
        //set_xon();
        break;
      case 81: //down
        Serial.write('\e');
        Serial.write('[');
        Serial.write('B');
        break; 
      case 82: //up
        Serial.write('\e');
        Serial.write('[');
        Serial.write('A');
        break;
      default:
        Serial.print((char)OemToAscii(mod, key));
    }
  }
};
