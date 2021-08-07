#include <pin_magic.h>
//let the process do computations when waiting for reading data

#define readR(result, lineData, color16, i)           \
{                                                     \
  RD_ACTIVE;                                          \
  color16 |= (result>>3);                             \
  lineData[i] = color16;                              \
  result = (PIND & B11111100) | (PINB & B00000011);   \
  RD_IDLE;                                            \
}
#define readG(result, color16)                        \
{                                                     \
  RD_ACTIVE;                                          \
  color16 = (result&0xF8) << 8;                       \
  result = (PIND & B11111100) | (PINB & B00000011);   \
  RD_IDLE;                                            \
}
#define readB(result, color16)                        \
{                                                     \
  RD_ACTIVE;                                          \
  color16 |= (result&0xFC) << 3;                      \
  result = (PIND & B11111100) | (PINB & B00000011);   \
  RD_IDLE;                                            \
}
