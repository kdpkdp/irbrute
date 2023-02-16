#ifndef __UTILS_TIME_H__
#define __UTILS_TIME_H__

#include <Arduino.h>

namespace Utils
{
namespace Time
{
  using Millis = unsigned long;
  using Micros = unsigned long;

  inline
  static Millis millisSinceMoment(Millis moment, Millis t)
  {
    return (moment >= t)
      ? (moment - t)
      : (Millis(-1) - t + moment);
  }

  inline
  static Millis millisSince(Millis t)
  {
    return millisSinceMoment(millis(), t);
  }

  inline
  static void waitMicros(Micros dur)
  {
    const Micros start = micros();
    while (micros() - start < dur)
    {
    }
  }

}
}

#endif
