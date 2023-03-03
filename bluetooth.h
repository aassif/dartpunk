#ifndef __DARTPUNK_BLUETOOTH__
#define __DARTPUNK_BLUETOOTH__

#include "image.h"

namespace dartpunk
{
  class App;

  class Bluetooth
  {
    private:
      Image logo;

    public: 
      Bluetooth ();
      void render (App *);
  };
}

#endif // __DARTPUNK_BLUETOOTH__

