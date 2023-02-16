#ifndef __CONTROLS_ERRORS_H__
#define __CONTROLS_ERRORS_H__

#include "../utils/result.h"

namespace Controls
{
  inline
  StringResult errorUnknownCommand(const String& cmd)
  {
    return StringResult::error(String(F("Unknown command: ")) + cmd);
  }

  inline
  StringResult errorRejectedCommand(const String& cmd)
  {
    return StringResult::error(String(F("Rejected command: ")) + cmd);
  }

  inline
  StringResult errorUnhandledCommand(const String& cmd)
  {
    return StringResult::error(String(F("Unhandled command: ")) + cmd);
  }
}

#endif
