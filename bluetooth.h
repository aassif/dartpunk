#ifndef __ED900_BLUETOOTH__
#define __ED900_BLUETOOTH__

#include "image.h"

namespace ed900
{
  class App;

  class Bluetooth
  {
    private:
      App * app;
      Image logo;

    public: 
      Bluetooth (App *);
      void render ();
  };
}

#endif // __ED900_BLUETOOTH__

