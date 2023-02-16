#ifndef __SIGNALS_IRREMOTE_SIGNAL_H__
#define __SIGNALS_IRREMOTE_SIGNAL_H__

#include "../../utils/result.h"

#include <string_view>

namespace IrRemote
{
  class IrSend;

  using EncodedSignalView = std::string_view;

  class Signal
  {
  public:
    virtual ~Signal() = default;

    virtual StringResult send(IrSend& sender, const EncodedSignalView& signal) = 0;
  };
}

#endif
