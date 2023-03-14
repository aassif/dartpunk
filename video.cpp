#include <chrono>

#include "app.h"
#include "video.h"

using namespace std;
using namespace chrono;

namespace dartpunk
{

  Video::Video (const string & path, uint8_t f) :
    logo {"images/" + path},
    frames {f}
  {
  }

  void Video::render (App * app)
  {
    static auto t0 = steady_clock::now ();
    auto dt = duration_cast<milliseconds> (steady_clock::now () - t0);
    uint8_t k = (dt.count () / 40) % frames;
    app->draw (logo, {0, 64 * k, 128, 64}, {0, 0}, blend::NONE);
  }

} // dartpunk

