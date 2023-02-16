#ifndef __APP_H__
#define __APP_H__

#include "signals/signals.h"
#include "controls/errors.h"
#include "utils/beeps.h"
#include "server.h"

template <typename Cfg>
class App
{
  WedServer<Cfg>            m_server;
  Signals::SignalsMode<Cfg> m_signals;

public:
  void setup()
  {
    Cfg::setupPins();

    Serial.begin(Cfg::SERIAL_BAUDS);
    while (!Serial)
    {
      // wait for serial port to connect. Needed for native USB port only
    }

    m_server.start([this](auto& s)
    {
      s.on("/", "text/html", [this]()                        { return StringResult::ok(m_server.getRootPage());     });
      s.onCmd("/exec/",      [this](String cmd, String data) { return execCommand(std::move(cmd), std::move(data)); });
      s.serveStatic("/ui", "/ui");
    });

    m_signals.start();
  }

  void loop()
  {
    m_server.loop();
    m_signals.loop();
  }

private:
  StringResult execCommand(String cmd, String data)
  {
    play(Utils::Beeps::melodyCmd());

    auto res = m_signals.handleCommand(cmd, data);
    if (!res.isHandled)
    {
      play(Utils::Beeps::melodyReject());
      return Controls::errorUnhandledCommand(cmd);
    }

    if (res.result.isError())
    {
      play(Utils::Beeps::melodyError());
    }

    return std::move(res.result);
  }

  static void play(const Utils::Beeps::BeepsMelody& melody)
  {
    Utils::Beeps::playSoftware(Cfg::PIN_BEEP, melody);
  }
};

#endif
