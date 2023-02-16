#ifndef __SIGNALS_IRREMOTE_SIGNAL_NEC_H__
#define __SIGNALS_IRREMOTE_SIGNAL_NEC_H__

#include "signal.h"

#include "../../utils/string.h"

#include "irremote_send.h"

namespace IrRemote
{
  class SignalNec : public Signal
  {
  public:
    using SendMethod = StringResult (*)(IrSend&, const EncodedSignalView&);

  private:
    const SendMethod m_sendMethod;

  public:
    SignalNec(SendMethod method)
      : m_sendMethod(method)
    {
    }

    virtual StringResult send(IrSend& sender, const EncodedSignalView& signal) override
    {
      return m_sendMethod(sender, signal);
    }

  private:
    static StringResult sendNec(IrSend& sender, const EncodedSignalView& signal)
    {
      struct Decoded
      {
        uint16_t     address = 0;
        uint8_t      command = 0;
        uint_fast8_t numberOfRepeats = 0;
        bool         sendOnlySpecialNecRepeat = false;
      };

      Decoded d;

      auto res = decode(signal, d);
      if (res.isError())
      {
        return res;
      }

      sender.sendNEC(d.address, d.command, d.numberOfRepeats, d.sendOnlySpecialNecRepeat);

      return StringResult::ok();
    }

    static StringResult sendNec2(IrSend& sender, const EncodedSignalView& signal)
    {
      struct Decoded
      {
        uint16_t     address = 0;
        uint8_t      command = 0;
        uint_fast8_t numberOfRepeats = 0;
        bool         sendOnlySpecialNecRepeat = false;
      };

      Decoded d;

      auto res = decode(signal, d);
      if (res.isError())
      {
        return res;
      }

      sender.sendNEC2(d.address, d.command, d.numberOfRepeats);

      return StringResult::ok();
    }

    static StringResult sendApple(IrSend& sender, const EncodedSignalView& signal)
    {
      struct Decoded
      {
        uint8_t     address = 0;
        uint8_t      command = 0;
        uint_fast8_t numberOfRepeats = 0;
        bool         sendOnlySpecialNecRepeat = false;
      };

      Decoded d;

      auto res = decode(signal, d);
      if (res.isError())
      {
        return res;
      }

      sender.sendApple(d.address, d.command, d.numberOfRepeats, d.sendOnlySpecialNecRepeat);

      return StringResult::ok();
    }

    static StringResult sendOnkyo(IrSend& sender, const EncodedSignalView& signal)
    {
      struct Decoded
      {
        uint16_t     address = 0;
        uint16_t     command = 0;
        uint_fast8_t numberOfRepeats = 0;
        bool         sendOnlySpecialNecRepeat = false;
      };

      Decoded d;

      auto res = decode(signal, d);
      if (res.isError())
      {
        return res;
      }

      sender.sendOnkyo(d.address, d.command, d.numberOfRepeats, d.sendOnlySpecialNecRepeat);

      return StringResult::ok();
    }

    template<typename Decoded>
    static StringResult decode(const EncodedSignalView& encoded, Decoded& decoded)
    {
      EncodedSignalView encodedValues[4] = {};
      size_t count = 0;
      auto res = Utils::splitTrimmed(encoded.data(), encoded.size(), ' ', [&](const EncodedSignalView& value)
      {
        if (count >= sizeof(encodedValues) / sizeof(*encodedValues))
        {
          return StringResult::error(String(F("Too many values")) + Utils::makeString(encoded));
        }
        encodedValues[count] = value;
        ++count;
        return StringResult::ok();
      });

      if (res.isError())
      {
        return res;
      }

      if (count < 3)
      {
        return StringResult::error(String(F("Bad NEC signal: ")) + Utils::makeString(encoded));
      }

      auto ev = encodedValues;
      if (!Utils::parseHex(ev->data(), ev->size(), true, decoded.address))
      {
        return StringResult::error(String(F("Failed to parse address: ")) + Utils::makeString(*ev));
      }

      ++ev;
      if (!Utils::parseHex(ev->data(), ev->size(), true, decoded.command))
      {
        return StringResult::error(String(F("Failed to parse command: ")) + Utils::makeString(*ev));
      }

      ++ev;
      if (!Utils::parseUnsigned(ev->data(), ev->size(), decoded.numberOfRepeats))
      {
        return StringResult::error(String(F("Failed to parse numberOfRepeats: ")) + Utils::makeString(*ev));
      }

      ++ev;
      if (count == 4)
      {
        if (!Utils::parseBool(ev->data(), ev->size(), decoded.sendOnlySpecialNecRepeat))
        {
          return StringResult::error(String(F("Failed to parse sendOnlySpecialNecRepeat: ")) + Utils::makeString(*ev));
        }
      }

      return StringResult::ok();
    }

  public:
    static constexpr SendMethod SEND_NEC       = &sendNec;
    static constexpr SendMethod SEND_NEC2      = &sendNec2;
    static constexpr SendMethod SEND_NEC_APPLE = &sendApple;
    static constexpr SendMethod SEND_NEC_ONKYO = &sendOnkyo;
  };
}

#endif
