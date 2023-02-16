#ifndef __UTILS_PINS_H__
#define __UTILS_PINS_H__

#include "time.h"
#include "pins/pins.h"

#include <Arduino.h>

namespace Utils
{
namespace Pins
{
  using Pin = PinsDecl::Pin;

  enum class PinMode : uint8_t
  {
    INPUT_MODE        = INPUT,
    OUTPUT_MODE       = OUTPUT,
    INPUT_PULLUP_MODE = INPUT_PULLUP,
  };

  inline
  constexpr uint8_t pin_cast(Pin pin) noexcept
  {
    return static_cast<uint8_t>(pin);
  }

  inline
  constexpr uint8_t pin_mode_cast(PinMode mode) noexcept
  {
    return static_cast<uint8_t>(mode);
  }

  inline
  void setPinState(Pin pin, bool state)
  {
    digitalWrite(pin_cast(pin), state);
  }

  template<bool STATE>
  void setPinState(Pin pin)
  {
    digitalWrite(pin_cast(pin), STATE);
  }

  inline
  bool getPinState(Pin pin)
  {
    return digitalRead(pin_cast(pin)) == HIGH;
  }

  inline
  void setPinMode(Pin pin, PinMode mode)
  {
    pinMode(pin_cast(pin), pin_mode_cast(mode));
  }

  template<PinMode MODE>
  void setPinMode(Pin pin)
  {
    pinMode(pin_cast(pin), pin_mode_cast(MODE));
  }
}
}

#endif
