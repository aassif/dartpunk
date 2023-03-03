#ifndef __DARTPUNK_DISPLAY_VIRTUAL__
#define __DARTPUNK_DISPLAY_VIRTUAL__

#include <SDL2/SDL.h>

#include "../app.h"

namespace dartpunk::display
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

} // dartpunk::display

#endif // __DARTPUNK_DISPLAY_VIRTUAL__

