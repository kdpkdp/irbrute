#ifndef __CONTROLS_MODE_H__
#define __CONTROLS_MODE_H__

#include "cmds.h"
#include "control.h"
#include "../utils/beeps.h"

namespace Controls
{
  template<typename Cfg>
  class Mode : public Control
  {
    Mode(const Mode&) = delete;
    Mode& operator = (const Mode&) = delete;

  public:
    using VoidListener = std::function<void()>;
    using BeepsMelody  = Utils::Beeps::BeepsMelody;

  private:
    VoidListener m_onDone;

  public:
    Mode()
    {
    }

    void setOnDone(const VoidListener& listener)
    {
      m_onDone = listener;
    }

    void done()
    {
      m_onDone();
    }

    CmdResult handleCommand(const String& cmd, const String& data)
    {
      if (isStarted())
      {
        return onModeCmd(cmd, data);
      }
      return CmdResult();
    }

    void play(const BeepsMelody& melody) const
    {
      Utils::Beeps::playSoftware(Cfg::PIN_BEEP, melody);
    }

  protected:
    virtual void onLoop() final
    {
      onModeLoop();
    }

    virtual void onStart() final
    {
      onModeStart();
    }

    virtual void onStop() final
    {
      onModeStop();
    }

    virtual void onModeLoop() = 0;
    virtual void onModeStart() = 0;
    virtual void onModeStop() = 0;
    virtual CmdResult onModeCmd(const String& cmd, const String& data) = 0;
  };
}

#endif
