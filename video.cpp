#include <chrono>

#include "app.h"
#include "video.h"

using namespace std;
using namespace chrono;

namespace dartpunk
{

  Video::Video (const string & path, uint8_t f, uint8_t d) :
    image {"images/" + path},
    frames {f},
    divisor {d}
  {
  }

  void Video::render (App * app, const OptDuration & d)
  {
    if (d)
    {
      uint16_t k = (*d / (divisor * 40ms)) % frames;
      app->draw (image, {0, HEIGHT * k, WIDTH, HEIGHT}, {0, 0});
    }
  }

} // dartpunk

