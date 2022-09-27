#include "app.h"
#include "bluetooth.h"

namespace ed900
{
  Bluetooth::Bluetooth (App * app) :
    app {app},
    logo {app->load ("images/bluetooth.png")}
  {
  }

  void Bluetooth::render ()
  {
    static const SDL_Rect R = {48, 3, 31, 57};
    app->draw (logo, R);

    float t = SDL_GetTicks () / 500.0;
    uint8_t c = static_cast<uint8_t> (127.5 + 127.5 * sin (t));
    app->draw (R, SDL_Color {c, c, 255, 255}, SDL_BLENDMODE_MOD);
  }
}

