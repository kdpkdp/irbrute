#ifndef __SIGNALS_SIGNALS_MODE__
#define __SIGNALS_SIGNALS_MODE__

#include "irremote/sender.h"
#include "../controls/mode.h"

namespace Signals
{
  namespace Sender = IrRemote;

  template <typename Cfg>
  class SignalsMode : public Controls::Mode<Cfg>
  {
    using ThisType = SignalsMode;
    using Cmds     = Controls::Cmds<2>;
    using Sender   = Sender::Sender<Cfg>;

    Sender m_sender;
    Cmds   m_cmds;

  public:
    SignalsMode()
    {
      m_cmds.set(Cmds::CmdEntryArr{
        Controls::CmdEntry("ir/info", std::bind(&ThisType::onCmdInfo, this, std::placeholders::_1, std::placeholders::_2)),
        Controls::CmdEntry("ir/send", std::bind(&ThisType::onCmdSend, this, std::placeholders::_1, std::placeholders::_2)),
      });
    }

  private:
    virtual void onModeLoop() override
    {
    }

    virtual void onModeStart() override
    {
      this->play(Utils::Beeps::melodyReady());
      m_sender.start();
    }

    virtual void onModeStop() override
    {
    }

    virtual Controls::CmdResult onModeCmd(const String& cmd, const String& data) override
    {
      return m_cmds(cmd, data);
    }

    Controls::CmdResult onCmdInfo(const String&, const String&)
    {
      return Controls::cmdResult(m_sender.getInfo());
    }

    Controls::CmdResult onCmdSend(const String&, const String& data)
    {
      return Controls::cmdResult(m_sender.send(data));
    }
  };
}

#endif
