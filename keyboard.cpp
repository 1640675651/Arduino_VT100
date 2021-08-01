#include "keyboard.h"

void KbdRptParser::OnKeyDown(uint8_t mod, uint8_t key)
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
      break;
    case 80: //left
      Serial.write('\e');
      Serial.write('[');
      Serial.write('D');
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
};
