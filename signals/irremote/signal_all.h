#ifndef __SIGNALS_IRREMOTE_SIGNAL_ALL_H__
#define __SIGNALS_IRREMOTE_SIGNAL_ALL_H__

#include "signal_delay.h"
#include "signal_nec.h"
#include "signal_raw.h"

#include <utility>
#include <memory>
#include <array>

namespace IrRemote
{
  auto createAllSignals()
  {
    using Elem = std::pair<String, std::unique_ptr<Signal>>;
    return std::array<Elem, 6>{
      std::make_pair(String(F("Delay")),    std::make_unique<SignalDelay>()),
      std::make_pair(String(F("Nec")),      std::make_unique<SignalNec>(SignalNec::SEND_NEC)),
      std::make_pair(String(F("Nec2")),     std::make_unique<SignalNec>(SignalNec::SEND_NEC2)),
      std::make_pair(String(F("NecApple")), std::make_unique<SignalNec>(SignalNec::SEND_NEC_APPLE)),
      std::make_pair(String(F("NecOnkyo")), std::make_unique<SignalNec>(SignalNec::SEND_NEC_ONKYO)),
      std::make_pair(String(F("Raw")),      std::make_unique<SignalRaw>()),
    };
  }
}

#endif
