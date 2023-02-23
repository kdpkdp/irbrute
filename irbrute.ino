#include "app.h"
#include "utils/pins.h"

struct Config
{
  static constexpr const char NET_NAME[] = "set_your_net_name";
  static constexpr const char NET_PASS[] = "set_your_net_pass";
  static constexpr const char AP_SSID[] = "irbrute";
  static constexpr const char AP_PASS[] = "irbrute";

  static constexpr unsigned PORT = 80;
  static constexpr unsigned SERIAL_BAUDS = 115200;

  static constexpr unsigned ATTEMPTS_TO_START_WIFI = 10;

  static constexpr auto PIN_BEEP      = Utils::Pins::Pin::D2;
  static constexpr auto PIN_INDICATOR = Utils::Pins::Pin::D5;
  static constexpr auto PIN_SEND      = Utils::Pins::Pin::D6;

  static inline void setupPins()
  {
    Utils::Pins::setPinMode<Utils::Pins::PinMode::OUTPUT_MODE>(PIN_INDICATOR);
    Utils::Pins::setPinMode<Utils::Pins::PinMode::OUTPUT_MODE>(PIN_BEEP);
    Utils::Pins::setPinMode<Utils::Pins::PinMode::OUTPUT_MODE>(PIN_SEND);
  }

  static_assert((sizeof(NET_NAME) > 1 || sizeof(AP_SSID) > 1) && "NET_NAME or AP_SSID should be specified");
};

App<Config> app;

void setup()
{
  app.setup();
}

void loop()
{
  app.loop();
}
