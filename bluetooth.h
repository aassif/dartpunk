#ifndef __ED900_BLUETOOTH__
#define __ED900_BLUETOOTH__

#include "image.h"

namespace ed900
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

#endif // __ED900_BLUETOOTH__

