Prerequisite Libraries: USBHost, USB Host Shield Library, Adafruit GFX Library, Adafruit_TFTLCD

Library modifications:
1) In the USB Host Shield Library, UsbCore.h find line

  typedef MAX3421e<P10, P9> MAX3421E; // Official Arduinos (UNO, Duemilanove, Mega, 2560, Leonardo, Due etc.), Intel Edison, Intel Galileo 2 or Teensy 2.0 and 3.x
  
Change P9 to a spare pin, for example P19 (A5). P9 will conflict with LCD data pin.

2) In the Adafruit_TFTLCD library, Adafruit_TFTLCD.cpp and .h, find function definition

  void Adafruit_TFTLCD::pushColors(uint16_t *data, uint8_t len, boolean first)

Change uint8_t len to uint16_t len. This is for line copying used by scrolling.

Hardware modification:

Connect int pin of the usb host shield to an unused pin, since the default P9 will be used by the LCD.

Host machine setting (Linux as example):

If we open a login shell directly on the serial, software flow control xon/xoff characters are not respected by the OS.
To bypass the xon/xoff limitation, we need to first create a virtual tty that respects xon/xoff signals and connect the real serial to the virtual tty.

sudo socat -d -d PTY,link=/tmp/vtty,raw,echo=0 /dev/ttyACM0,b4800,raw,echo=0

sudo setsid agetty pts/2 4800 vt100 (The pts/2 number may be different, see the output of socat)

Set terminal size after connection:

stty rows 30 cols 53 (Landscape mode)
stty rows 40 cols 40 (Portrait mode)

Limitations:

Screen scrolling is very slow since we need to read video memory from the ILI9341 controller. Theoretically we can use software flow control to backpressure the host machine when the serial buffer is pressured. However, even with software flow control, the host machine can still send over a lot of data after xoff. In my experiments, the host will send roughly up to 4KB data after the arduino initiates xoff. This is likely due to the usb to serial driver and usb protocol behavior. The OS can commit a lot of data to the usb transmission buffer, and the commited data cannot be recalled when OS receives xoff. Therefore, serial buffer overflow still happens and data can be dropped when the screen scrolls a lot. Lowering baud rate or increasing serial buffer size helps a bit but cannot completely solve the problem. The flow control should work better with real serial hardware, like the raspbery pi serial ports.

Line wrapping at the last line is not implemented, since it requires scrolling. Now we just move the cursor to the beginning of the row.

Similarly due to buffering, when you use ctrl-C to terminate the current program, the OS need to flush all buffered outputs. The program terminates immediately when receiving ctrl-C, but arduino will get a lot of data after the program terminates.

Screen scrolling only happens at explicit scrolling command and newline characters on the last row of the scrolling region.

Show cursor is not implmented.
