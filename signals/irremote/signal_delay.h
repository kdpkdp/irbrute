#ifndef __SIGNALS_IRREMOTE_SIGNAL_DELAY_H__
#define __SIGNALS_IRREMOTE_SIGNAL_DELAY_H__

#include "signal.h"

#include "../../utils/time.h"
#include "../../utils/string.h"

#include "irremote_send.h"

namespace IrRemote
{
  class SignalDelay : public Signal
  {
    static constexpr Utils::Time::Millis DEFAULT_DELAY_MILLIS = 1000;

  public:
    virtual StringResult send(IrSend&, const EncodedSignalView& signal) override
    {
      auto delayMillis = DEFAULT_DELAY_MILLIS;
      if (!signal.empty())
      {
        if (!Utils::parseUnsigned(signal.data(), signal.size(), delayMillis))
        {
          return StringResult::error(String(F("Failed to parse delay: ")) + Utils::makeString(signal));
        }
      }

      delay(delayMillis);

      return StringResult::ok();
    }
  };
}

#endif
