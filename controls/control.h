#ifndef __CONTROLS_CONTROL_H__
#define __CONTROLS_CONTROL_H__

#include "../utils/pins.h"
#include "../utils/time.h"

namespace Controls
{
  class Control
  {
    Control(const Control&) = delete;
    Control& operator = (const Control&) = delete;

    Utils::Time::Millis m_startTime = 0;
    bool                m_isStarted = false;

  public:
    Control() = default;
    virtual ~Control() = default;

    bool isStarted() const noexcept
    {
      return m_isStarted;
    }

    Utils::Time::Millis millisSinceStart() const
    {
      return m_isStarted ? Utils::Time::millisSince(m_startTime) : 0;
    }

    void loop()
    {
      if (m_isStarted)
      {
        onLoop();
      }
    }

    void start()
    {
      setStarted(true);
    }

    void stop()
    {
      setStarted(false);
    }

    void setStarted(bool isStarted)
    {
      if (isStarted != m_isStarted)
      {
        m_isStarted = isStarted;
        if (isStarted)
        {
          m_startTime = millis();
          onStart();
        }
        else
        {
          onStop();
        }
      }
    }

  protected:
    virtual void onLoop() = 0;
    virtual void onStart() = 0;
    virtual void onStop() = 0;
  };
}

#endif
