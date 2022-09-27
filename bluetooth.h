#ifndef __ED900_BLUETOOTH__
#define __ED900_BLUETOOTH__

#include <SDL2/SDL.h>

namespace ed900
{
  class App;

  class Bluetooth
  {
    private:
      App * app;
      SDL_Texture * logo;

    public: 
      Bluetooth (App *);
      void render ();
  };
}

#endif // __ED900_BLUETOOTH__

