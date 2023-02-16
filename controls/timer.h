#ifndef __CONTROLS_TIMER_H__
#define __CONTROLS_TIMER_H__

#include "control.h"

namespace Controls
{
  class Timer final : public Control
  {
  public:
    using Listener = NvHandler<void>;
    using Count    = unsigned;

    static constexpr Count UNLIMITED = Count(-1);

  private:
    Listener m_listener;
    Millis   m_lastTickTime = 0;
    Millis   m_period = 0;
    Count    m_ticks = 0;
    Count    m_ticksLimit = UNLIMITED;

  public:
    void setTicksLimit(Count ticksLimit)
    {
      m_ticksLimit = ticksLimit;
      onTicksChanged();
    }

    void setPeriod(Millis period)
    {
      m_period = period;
    }

    void setListener(const Listener& listener)
    {
      m_listener = listener;
    }

  protected:
    virtual void onLoop() final
    {
      const auto ms = millisSinceStart();
      const auto sinceLastTick = millisSinceMoment(ms, m_lastTickTime);
      if (sinceLastTick > m_period)
      {
        m_lastTickTime = ms;
        ++m_ticks;
        onTicksChanged();
        m_listener.handle();
      }
    }

    virtual void onStart() final
    {
      m_ticks = 0;
      m_lastTickTime = 0;
      onTicksChanged();
    }

    virtual void onStop() final
    {
    }

  private:
    void onTicksChanged()
    {
      if (m_ticks >= m_ticksLimit && m_ticksLimit != UNLIMITED)
      {
        stop();
      }
    }
  };
}

#endif
