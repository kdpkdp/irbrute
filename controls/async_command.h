#ifndef __CONTROLS_ASYNC_COMMAND_H__
#define __CONTROLS_ASYNC_COMMAND_H__

#include "control.h"

namespace Controls
{
  class AsyncCommand final : public Control
  {
  public:
    using Command = NvHandler<void>;

  private:
    bool    m_execute = false;
    Command m_command;

  public:
    void setCommand(const Command& command)
    {
      m_command = command;
    }

    void execute()
    {
      m_execute = true;
    }

    void execute(const Command& command)
    {
      setCommand(command);
      execute();
    }

  protected:
    virtual void onLoop() final
    {
      if (m_execute)
      {
        m_execute = false;
        m_command.handle();
      }
    }

    virtual void onStart() final
    {
    }

    virtual void onStop() final
    {
      m_execute = false;
    }
  };
}

#endif
