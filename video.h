#ifndef __DARTPUNK_VIDEO__
#define __DARTPUNK_VIDEO__

#include <chrono>
#include <optional>

#include "image.h"

namespace dartpunk
{
  class App;

  class Video
  {
    public:
      typedef std::chrono::steady_clock::duration Duration;
      typedef std::optional<Duration> OptDuration;

    private:
      Image image;
      uint8_t frames;
      uint8_t divisor;

    public:
      Video (const std::string &, uint8_t f, uint8_t d = 1);
      void render (App *, const OptDuration &);
  };
}

#endif // __DARTPUNK_VIDEO__

