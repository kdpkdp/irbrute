#ifndef __SIGNALS_IRREMOTE_SIGNAL_RAW_H__
#define __SIGNALS_IRREMOTE_SIGNAL_RAW_H__

#include "signal.h"

#include "../../utils/string.h"
#include "../../utils/time.h"

#include <vector>

#include "irremote_send.h"

#include <algorithm>

namespace IrRemote
{
  class SignalRaw : public Signal
  {
    static constexpr size_t MAX_BUFFER_SIZE = 128;
    static constexpr Utils::Time::Millis DELAY_AFTER_SEND_MILLIS = 50;
    static constexpr char VALUE_SEPARATOR = '|';

    using Buffer = std::vector<uint32_t>;

    Buffer m_buffer;

  public:
    virtual StringResult send(IrSend& sender, const EncodedSignalView& signal) override
    {
      auto res = decode(signal, m_buffer);
      if (res.isError())
      {
        return res;
      }

      const uint32_t freq = m_buffer[0];
      const uint32_t* durations = &m_buffer[1];
      const size_t durationsCount = m_buffer.size() - 1;
      res = sender.sendRaw(durations, durationsCount, freq);
      if (res.isError())
      {
        return res;
      }

      delay(DELAY_AFTER_SEND_MILLIS);

      if (m_buffer.size() > MAX_BUFFER_SIZE)
      {
        m_buffer.resize(MAX_BUFFER_SIZE);
      }

      return StringResult::ok();
    }

    static StringResult decode(const EncodedSignalView& encoded, Buffer& buffer)
    {
      const auto separators = std::count(encoded.begin(), encoded.end(), VALUE_SEPARATOR);
      if (separators < 2) // 1 for frequency + 1 for duration
      {
        return StringResult::error(String(F("Not enough data to decode signal from string: ")) + Utils::makeString(encoded));
      }

      if (encoded[encoded.size() - 1] != VALUE_SEPARATOR)
      {
        return StringResult::error(String(F("Data is not ended with the '")) + VALUE_SEPARATOR + F("': ") + Utils::makeString(encoded));
      }

      buffer.resize(separators);

      size_t count = 0;
      auto res = Utils::splitTrimmed(encoded.data(), encoded.size(), VALUE_SEPARATOR, [&buffer, &count](const EncodedSignalView& value)
      {
        if (buffer.size() < count)
        {
          return StringResult::error(String(F("Out of range")));
        }

        if (!Utils::parseUnsigned(value.data(), value.size(), buffer[count]))
        {
          return StringResult::error(String(F("Failed to parse value: ")) + Utils::makeString(value));
        }

        ++count;

        return StringResult::ok();
      });

      return res;
    }
  };
}

#endif
