#ifndef __IRREMOTE_SEND_H__
#define __IRREMOTE_SEND_H__

#include "../../utils/result.h"

#include "irremote_defs.h"

namespace IrRemote
{
  class IrSend : public IRsend
  {
  public:
    static constexpr uint8_t PIN_SEND = IR_SEND_PIN;
    static constexpr char VERSION[] = VERSION_IRREMOTE;

    static StringResult validateRaw(const uint32_t* aBufferWithMicroseconds, uint32_t aLengthOfBuffer)
    {
      for (uint32_t i = 0; i < aLengthOfBuffer; ++i, ++aBufferWithMicroseconds)
      {
        if (!(i & 1))
        {
          if (*aBufferWithMicroseconds > 0xffff)
          {
            return StringResult::error(F("Mark is exceeded max value."));
          }
        }
      }

      return StringResult::ok();
    }

    StringResult sendRaw(const uint32_t* aBufferWithMicroseconds, uint32_t aLengthOfBuffer, uint32_t aFrequency)
    {
      const auto res = validateRaw(aBufferWithMicroseconds, aLengthOfBuffer);
      if (res.isError())
      {
        return res;
      }

      enableIROut(uint_fast8_t(aFrequency / 1000));

      for (uint_fast16_t i = 0; i < aLengthOfBuffer; i++)
      {
        if (i & 1) {
          space32(aBufferWithMicroseconds[i]);
        } else {
          mark(aBufferWithMicroseconds[i]);
        }
      }

      IrReceiver.restartAfterSend();

      return StringResult::ok();
    }

    void space32(uint32_t miscros)
    {
      customDelayMicroseconds(miscros);
    }

    void customDelayMicroseconds32(uint32_t miscros)
    {
      using CustomDelayFn = void (IRsend::*)(unsigned long);
      static_assert(CustomDelayFn(&IRsend::customDelayMicroseconds)); // checks required signature
      static_assert(sizeof(miscros) <= sizeof(unsigned long));
      customDelayMicroseconds(miscros);
    }
  };
}

#endif
