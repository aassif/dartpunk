#ifndef __ED900_DISPLAY_VIRTUAL__
#define __ED900_DISPLAY_VIRTUAL__

#include <SDL2/SDL.h>

#include "../app.h"

namespace ed900::display
{

  class Virtual
  {
    private:
      SDL_Window   * window;
      SDL_Renderer * renderer;
      SDL_Texture  * texture;

    public:
      Virtual (uint8_t scale = 5);
      ~Virtual ();

      void operator() (const App::Matrix &);
  };

}

#endif // __ED900_DISPLAY_VIRTUAL__

