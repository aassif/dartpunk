#include "app.h"
#include "bluetooth.h"

namespace ed900
{
  Bluetooth::Bluetooth (App * app) :
    app {app},
    logo {"images/bluetooth.png"}
  {
  }

  void Bluetooth::render ()
  {
    static const uint8_t X = 48;
    static const uint8_t Y = 3;
    static const uint8_t W = 31;
    static const uint8_t H = 57;

    app->draw (logo, {0, 0, W, H}, {X, Y});

    float t = SDL_GetTicks () / 500.0;
    uint8_t c = static_cast<uint8_t> (127.5 + 127.5 * sin (t));
    app->draw ({X, Y, W, H}, Color {c, c, 255, 255}, blend::MODULATE);
  }
}

