#ifndef __SIGNALS_IRREMOTE_SENDER_H__
#define __SIGNALS_IRREMOTE_SENDER_H__

#include "signal_all.h"

#include "../../utils/beeps.h"
#include "../../utils/result.h"
#include "../../utils/pins.h"
#include "../../utils/string.h"

#include "irremote_send.h"

namespace IrRemote
{
  template <typename Cfg>
  class Sender
  {
    static_assert(Utils::Pins::pin_cast(Cfg::PIN_SEND) == IrSend::PIN_SEND);

    using KnownSignal     = std::pair<String, std::unique_ptr<Signal>>;
    using KnownSignalsArr = decltype(createAllSignals());

  private:
    IrSend          m_sender;
    KnownSignalsArr m_knownSignals;

  public:
    Sender()
      : m_sender()
      , m_knownSignals(createAllSignals())
    {
    }

    StringResult getInfo()
    {
      String info = "";

      info += String(F("\nIrRemote: ")) + IrSend::VERSION;
      info += String(F("\nSend pin: GPIO ")) + Utils::Pins::pin_cast(Cfg::PIN_SEND);
      info += String(F("\nProtocols [")) + m_knownSignals.size() + F("]:");
      for (const auto& signal: m_knownSignals)
      {
        info += String(F("\n  ")) + signal.first;
      }

      const int freqs[] = {36, 38, 40, 56};
      info += String(F("\n\nFrequencies [")) + (sizeof(freqs) / sizeof(*freqs)) + F("]:");
      for (unsigned i = 0; i < sizeof(freqs) / sizeof(*freqs); ++i)
      {
        m_sender.enableIROut(freqs[i]);
        info += String(F("\n  ")) + freqs[i] + F("KHz:");
        info += String(F("\n    Period duration:   ")) + m_sender.periodTimeMicros + F(" micros");
        info += String(F("\n    Pulse ON duration: ")) + m_sender.periodOnTimeMicros + F(" micros");
        info += String(F("\n    Pulse correction:  ")) + m_sender.getPulseCorrectionNanos() + F(" nanos");
      }

      return StringResult::ok(info);
    }

    StringResult send(const String& signal)
    {
      return Utils::splitTrimmed(signal.c_str(), signal.length(), END_OF_LINE, [this](const EncodedSignalView& signal)
      {
        return signal.empty() ? StringResult::ok() : sendImpl(signal);
      });
    }

    void start()
    {
      Utils::Pins::setPinState(Cfg::PIN_INDICATOR, true);
      m_sender.begin(Utils::Pins::pin_cast(Cfg::PIN_SEND));
    }

    void stop()
    {
      // do nothing
    }

  private:
    StringResult sendImpl(const EncodedSignalView& signal)
    {
      EncodedSignalView protocol;
      EncodedSignalView data;
      auto res = identifySignal(signal, protocol, data);
      if (res.isError())
      {
        return res;
      }

      Signal* s = findSignal(protocol);
      if (s == nullptr)
      {
        return StringResult::error(String(F("Unknown protocol: ")) + Utils::makeString(protocol));
      }

      Utils::Beeps::playSoftware(Cfg::PIN_BEEP, Utils::Beeps::melodyPew());

      Utils::Pins::setPinState(Cfg::PIN_INDICATOR, false);

      res = s->send(m_sender, data);

      Utils::Pins::setPinState(Cfg::PIN_INDICATOR, true);

      return res;
    }

    Signal* findSignal(const EncodedSignalView& protocol) const
    {
      for (const auto& signal : m_knownSignals)
      {
        const auto& signalProtocol = signal.first;
        if (signalProtocol.length() != protocol.size())
        {
          continue;
        }

        if (strncmp(signalProtocol.c_str(), protocol.data(), protocol.size()) == 0)
        {
          return signal.second.get();
        }
      }

      return nullptr;
    }

    static StringResult identifySignal(const EncodedSignalView& signal, EncodedSignalView& protocol, EncodedSignalView& data)
    {
      for (size_t i = 0, size = signal.size(); i != size; ++i)
      {
        if (signal[i] == END_OF_PROTOCOL)
        {
          const auto dataIdx = i + 1;
          if (dataIdx >= size)
          {
            return StringResult::error(String(F("Signal is without data: ")) + Utils::makeString(signal));
          }

          protocol = EncodedSignalView(&signal[0], i);
          data = EncodedSignalView(&signal[dataIdx], size - dataIdx);
          return StringResult::ok();
        }
      }
      return StringResult::error(String(F("Signal is without protocol: ")) + Utils::makeString(signal));
    }

    static constexpr char END_OF_LINE     = '\n';
    static constexpr char END_OF_PROTOCOL = ':';
  };
}

#endif
