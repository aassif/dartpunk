#ifndef __DARTPUNK_VIDEO__
#define __DARTPUNK_VIDEO__

#include "image.h"

namespace dartpunk
{
  class App;

  class Video
  {
    private:
      Image logo;
      uint8_t frames;

    public:
      Video (const std::string &, uint8_t);
      void render (App *);
  };
}

#endif // __DARTPUNK_VIDEO__

