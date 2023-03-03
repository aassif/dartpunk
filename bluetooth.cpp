#include <chrono>

#include "app.h"
#include "bluetooth.h"

using namespace std;
using namespace chrono;

namespace dartpunk
{

  Bluetooth::Bluetooth () :
    logo {"images/bluetooth.png"}
  {
  }

  void Bluetooth::render (App * app)
  {
    static const uint8_t X = 48;
    static const uint8_t Y = 3;
    static const uint8_t W = 31;
    static const uint8_t H = 57;

    static auto t0 = steady_clock::now ();
    auto dt = duration_cast<milliseconds> (steady_clock::now () - t0);
    uint8_t c = static_cast<uint8_t> (127.5 + 127.5 * sin (dt.count () / 500.0));
    app->draw (logo, {0, 0, W, H}, {X, Y}, blend::ModAlpha ({c, c, 255, 255}));
  }

} // dartpunk

