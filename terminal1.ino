#include "keyboard.h"
#include "lcd.h"
#include "vt100_ctl.h"

//#define SUPPORT_SCROLLING
// global variables
char char_from_serial = -1;
uint8_t cursor_x;
uint8_t cursor_y;
// USB objects
USB     Usb;
HIDBoot<USB_HID_PROTOCOL_KEYBOARD>    HidKeyboard(&Usb);
KbdRptParser Prs;

// LCD objects
Adafruit_TFTLCD tft(LCD_CS, LCD_CD, LCD_WR, LCD_RD, LCD_RESET);

void setup() {
  #ifdef SUPPORT_SCROLLING
  Serial.begin(1200);
  #else
  Serial.begin(4800);
  #endif
  lcdinit(tft);
  
  if (Usb.Init() == -1)
    Serial.println("OSC did not start.");
  HidKeyboard.SetReportParser(0, &Prs);

}

void loop() {
  // put your main code here, to run repeatedly:
  Usb.Task();
  if(Serial.available())
  {
    parse_serial_in(); 
    //debug();
  }
}

void parse_serial_in()
{
  char_from_serial = Serial.read();
  if(char_from_serial == '\e') //received VT100 control code
  {
    char ctlcode = -1;
    Serial.readBytes(&ctlcode, 1); //readBytes has a timeout, which can be used to wait for the next byte
    if(ctlcode == '[')
    {
      parse_ctl_code(tft);
    }
    else if(ctlcode == '(' || ctlcode == ')') //set character set commands, not implemented
    {
      Serial.readBytes(&ctlcode, 1);
    }
    else if(ctlcode == 'D') //index: move cursor down and scroll screen up if cursor at the bottom
    {
      moveCursorandScroll(1, tft);
    }
    else if(ctlcode == 'M') //revindex: move cursor up and scroll screen down if cursor at the top
    {
      //Serial.write(19); //pause host serial port
      moveCursorandScroll(-1, tft);
      //Serial.write(17);
    }
  }
  else if(char_from_serial == 0)// I don't know why the linux machine will send a lot of 0's 
  {                             // when backspace is pressed. Under 115200 baud rate, arduino 
    return;                     // will receive more than 40 0's and under 9600 baud rate,
  }                             // arduino will receive ~3 0's.
  else if(char_from_serial == 8)//move cursor backwards
  {
    moveCursor(-1, 0, tft);
  }
  else if(char_from_serial == 7)//bell
  {
    ;
  }
  else if(char_from_serial >= 14 && char_from_serial <= 31);// unimplemented control characters
  else if(char_from_serial < 0)// UTF-8 characters (unsupported, just ignore)
  {
    overWrite(' ', tft);
    char_from_serial <<= 1; //the first byte of UTF-8 character indicates how many bytes this character has
    while(char_from_serial & 0b10000000) //the number of bits that are 1 in the beginning is the number of bytes.
    {
      while(!Serial.available());
      Serial.read();
      char_from_serial <<= 1;
    }
  }
  else// normal characters
  {
    overWrite(char_from_serial, tft);
  }
}

void debug()
{
  char_from_serial = Serial.read();
  tft.print(char_from_serial);
  tft.print(' ');
  tft.print((uint8_t)char_from_serial);
  tft.print(' ');
}

/*
note:
usb host shield ??????spi????????????uno??????11???12???13?????????
usb host shield ????????????9???10???????????????spi???int???ss?????????
??????lcd????????????2~9?????????????????????9??????????????????lcd?????????????????????
????????????usb host shield????????????int??????????????????????????????A5?????????P19????????????????????????
?????????????????????usb host shield???????????????????????????????????????int???????????????????????????
github?????????
Furthermore it uses one pin as SS and one INT pin. These are by default located on pin 10 and 9 respectively. They can easily be reconfigured in case you need to use them for something else by cutting the jumper on the shield and then solder a wire from the pad to the new pin.

After that you need modify the following entry in UsbCore.h:

typedef MAX3421e<P10, P9> MAX3421E;
For instance if you have rerouted SS to pin 7 it should read:

typedef MAX3421e<P7, P9> MAX3421E;

another note:
Increased SERIAL_RX_BUFFER from 64 to 128 ((Arduino install path/hardware/arduino/avr/cores/arduino/HardwareSerial.h))
Limited baudrate to 4800
*/
