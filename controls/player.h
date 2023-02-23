#ifndef __CONTROLS_PAYER_H__
#define __CONTROLS_PAYER_H__

#include "control.h"
#include "timer.h"

#include "../utils/beeps.h"

namespace Controls
{
  class Player final : public Control
  {
  public:
    using ThisType    = Player;
    using Beep        = Utils::Beeps::Beep;
    using BeepsMelody = Utils::Beeps::BeepsMelody;

  private:
    struct State
    {
      BeepsMelody melody;
      Millis beepMs = 0;
      const Beep* beep = nullptr;
      uint8_t beepIdx = 0;
    };

  private:
    const  Pin m_pin;
    State  m_stateBuf;
    State* m_state;

  public:
    Player(Pin beepPin)
      : m_pin(beepPin)
      , m_state(nullptr)
    {
      setPinMode<PinMode::OUTPUT_MODE>(m_pin);
    }

    void play(const BeepsMelody& melody)
    {
      clear();

      if (melody.size <= 0)
      {
        return;
      }

      m_stateBuf.melody = melody;
      m_state = &m_stateBuf;
      setCurrentBeep(0);
    }

    void playSync(const BeepsMelody& melody)
    {
      clear();
      Utils::Beeps::playSoftware(m_pin, melody);
    }

    void clear()
    {
      m_state = nullptr;
      update();
    }

  protected:
    virtual void onLoop() final
    {
      update();
    }

    virtual void onStart() final
    {
      clear();
    }

    virtual void onStop() final
    {
      clear();
    }

  private:
    void update()
    {
      if (m_state == nullptr)
      {
        ::setPinNoTone(m_pin);
        return;
      }

      const auto dur = ::millisSince(m_state->beepMs);
      if (dur < m_state->beep->dur)
      {
        return;
      }

      setCurrentBeep(m_state->beepIdx + 1);
    }

    void setCurrentBeep(uint8_t beepIdx)
    {
      if (beepIdx >= m_state->melody.size)
      {
        clear();
        return;
      }

      m_state->beepMs = ::millis();
      m_state->beepIdx = beepIdx;
      m_state->beep = &m_state->melody.beeps[beepIdx];
      Utils::Beeps::setPinTone(m_pin, m_state->beep->freq);
    }
  };
}

#endif
