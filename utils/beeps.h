#ifndef __UTILS_BEEPS_H__
#define __UTILS_BEEPS_H__

#include "pins.h"
#include "time.h"

#include <Arduino.h>

namespace Utils
{
namespace Beeps
{
  using ToneFreq = unsigned;

  inline
  void setPinNoTone(Pins::Pin pin)
  {
    noTone(Pins::pin_cast(pin));
  }

  inline
  void setPinTone(Pins::Pin pin, ToneFreq freq)
  {
    const auto pinVal = Pins::pin_cast(pin);
    if (freq > 0)
    {
      tone(pinVal, freq);
    }
    else
    {
      noTone(pinVal);
    }
  }

  inline
  void setPinTone(Pins::Pin pin, bool isOn, ToneFreq freq)
  {
    setPinTone(pin, isOn ? freq : 0);
  }

  inline
  void playHardware(Pins::Pin pin, ToneFreq freq, Time::Millis durMillis)
  {
    setPinTone(pin, freq);
    delay(durMillis);
    setPinNoTone(pin);
  }

  inline
  void playSoftware(Pins::Pin pin, ToneFreq freq, Time::Millis durMillis)
  {
    if (durMillis == 0)
    {
      return;
    }

    const Time::Micros toneDur = 1000 * durMillis;
    if (freq == 0)
    {
      Time::waitMicros(toneDur);
      return;
    }

    const Time::Micros pulseDur = 1000000 / freq;
    if (pulseDur > toneDur)
    {
      Time::waitMicros(toneDur);
      return;
    }

    const Time::Micros highDur = pulseDur / 4;
    const Time::Micros lowDur = pulseDur - highDur;
    const Time::Millis start = millis();
    while (Time::millisSince(start) < durMillis)
    {
      Pins::setPinState<true>(pin);
      Time::waitMicros(highDur);
      Pins::setPinState<false>(pin);
      Time::waitMicros(lowDur);
    }
  }

  struct Beep
  {
    ToneFreq     freq = 0;
    Time::Millis dur = 0;

    constexpr Beep() = default;

    constexpr Beep(ToneFreq f, Time::Millis d)
      : freq(f)
      , dur(d)
    {
    }
  };

  struct BeepsMelody
  {
    const Beep* beeps = nullptr;
    uint8_t     size = 0;

    constexpr BeepsMelody() = default;

    constexpr BeepsMelody(const Beep* b, uint8_t s) noexcept
      : beeps(b)
      , size(s)
    {
    }
  };

  template<unsigned N>
  constexpr BeepsMelody makeBeepsMelody(const Beep(&beeps)[N]) noexcept
  {
    return BeepsMelody(&beeps[0], N);
  }

  inline
  void playHardware(Pins::Pin pin, const BeepsMelody& melody)
  {
    for (uint8_t i = 0; i != melody.size; ++i)
    {
      playHardware(pin, melody.beeps[i].freq, melody.beeps[i].dur);
    }
  }

  inline
  void playSoftware(Pins::Pin pin, const BeepsMelody& melody)
  {
    for (uint8_t i = 0; i != melody.size; ++i)
    {
      playSoftware(pin, melody.beeps[i].freq, melody.beeps[i].dur);
    }
  }

  namespace Melodies
  {
    constexpr Beep MELODY_IDLE[] = {
        Beep(150, 50)
    };

    constexpr Beep MELODY_MESSAGE[] = {
        Beep(500, 25)
      , Beep(1000, 25)
    };

    constexpr Beep MELODY_CLEAR[] = {
        Beep(1250, 50)
      , Beep(750, 50)
      , Beep(1250, 50)
    };

    constexpr Beep MELODY_REJECT[] = {
        Beep(150, 100)
      , Beep(0, 50)
      , Beep(75, 100)
    };

    constexpr Beep MELODY_BOOT[] = {
        Beep(200, 100)
      , Beep(0, 50)
      , Beep(200, 100)
    };

    constexpr Beep MELODY_READY[] = {
        Beep(2000, 100)
      , Beep(0, 100)
      , Beep(2000, 100)
      , Beep(0, 100)
      , Beep(2000, 100)
    };

    constexpr Beep MELODY_PEW[] = {
        Beep(250, 10)
    };

    constexpr Beep MELODY_CMD[] = {
        Beep(250, 10)
    };
  }

  constexpr BeepsMelody melodyIdle() noexcept
  {
    return makeBeepsMelody(Melodies::MELODY_IDLE);
  }

  constexpr BeepsMelody melodyMessage() noexcept
  {
    return makeBeepsMelody(Melodies::MELODY_MESSAGE);
  }

  constexpr BeepsMelody melodyClear() noexcept
  {
    return makeBeepsMelody(Melodies::MELODY_CLEAR);
  }

  constexpr BeepsMelody melodyReject() noexcept
  {
    return makeBeepsMelody(Melodies::MELODY_REJECT);
  }

  constexpr BeepsMelody melodyError() noexcept
  {
    return makeBeepsMelody(Melodies::MELODY_REJECT);
  }

  constexpr BeepsMelody melodyBoot() noexcept
  {
    return makeBeepsMelody(Melodies::MELODY_BOOT);
  }

  constexpr BeepsMelody melodyReady() noexcept
  {
    return makeBeepsMelody(Melodies::MELODY_READY);
  }

  constexpr BeepsMelody melodyPew() noexcept
  {
    return makeBeepsMelody(Melodies::MELODY_PEW);
  }

  constexpr BeepsMelody melodyCmd() noexcept
  {
    return makeBeepsMelody(Melodies::MELODY_CMD);
  }
}
}

#endif
