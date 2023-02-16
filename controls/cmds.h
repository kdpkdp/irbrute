#ifndef __CONTROLS_CMDS_H__
#define __CONTROLS_CMDS_H__

#include "../utils/result.h"

#include <array>
#include <utility>
#include <functional>

namespace Controls
{
  struct CmdResult
  {
    bool         isHandled = false;
    StringResult result;
  };

  CmdResult cmdResult(StringResult result)
  {
    return {true, std::move(result)};
  }

  template<typename ...Args>
  CmdResult cmdResultOk(Args&&... args)
  {
    return {true, StringResult::ok(std::forward<Args>(args)...)};
  }

  template<typename ...Args>
  CmdResult cmdResultError(Args&&... args)
  {
    return {true, StringResult::error(std::forward<Args>(args)...)};
  }

  struct CmdEntry
  {
    using Handler = std::function<CmdResult(const String&, const String&)>;

    const char* cmd = nullptr;
    size_t      cmdLen = 0;
    Handler     handler;

    CmdEntry() = default;

    CmdEntry(const char* c, Handler h)
      : cmd(c)
      , cmdLen(strlen(c))
      , handler(h)
    {
    }
  };

  template<typename CommandEntries>
  CmdResult handleCommand(const CommandEntries& entries, const String& cmd, const String& data)
  {
    for (const auto& entry : entries)
    {
      if (cmd.startsWith(entry.cmd))
      {
        return entry.handler(cmd.substring(entry.cmdLen), data);
      }
    }
    return CmdResult();
  }


  template<unsigned N>
  class Cmds
  {
  public:
    using CmdEntryArr = std::array<CmdEntry, N>;

    CmdEntryArr m_cmds;

  public:
    Cmds() = default;

    Cmds(const CmdEntryArr& cmds)
      : m_cmds(cmds)
    {
    }

    void set(const CmdEntryArr& cmds)
    {
      m_cmds = cmds;
    }

    CmdResult operator () (const String& cmd, const String& data)
    {
      return handleCommand(m_cmds, cmd, data);
    }

    CmdEntry makeCmdEntryFromThis(const char* cmd) const
    {
      return CmdEntry(cmd, *this);
    }
  };
}

#endif
