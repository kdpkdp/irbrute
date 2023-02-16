#ifndef __PINS_ESP_H__
#define __PINS_ESP_H__

#include <inttypes.h>

// https://randomnerdtutorials.com/esp8266-pinout-reference-gpios/
// https://lastminuteengineers.com/esp8266-pinout-reference/

enum class Esp12FPin : uint8_t
{
  LED = 2,
  _D0 = 16,  // !  HIGH at boot, used to wake up from deep sleep
  D1 = 5,   // ok
  D2 = 4,   // ok
  _D3 = 0,   // !  connected to FLASH button, boot fails if pulled LOW
  _D4 = 2,   // !  Led and HIGH at boot, boot fails if pulled LOW
  D5 = 14,  // ok
  D6 = 12,  // ok
  D7 = 13,  // ok
  _D8 = 15,  // !  Required for boot, boot fails if pulled HIGH
  _Rx = 3,   // X  Rx pin, used for flashing and debugging
  _Tx = 1,   // X  Tx pin, used for flashing and debugging
  _Clx = 6,  // X  Connected to Flash memory
  _Sdo = 7,  // X  Connected to Flash memory
  _Cmd = 11, // X  Connected to Flash memory
  _S1 = 8,   // X  Connected to Flash memory
  _S2 = 9,   // X  Connected to Flash memory
  _S3 = 10,  // X  Connected to Flash memory
};

#endif
