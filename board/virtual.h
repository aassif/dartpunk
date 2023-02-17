#ifndef __ED900_BOARD_VIRTUAL__
#define __ED900_BOARD_VIRTUAL__

#include <SDL2/SDL.h>

#include "board.h"

namespace ed900::board
{

  class Virtual : public Board
  {
    private:
      SDL_Window * window;
      SDL_Renderer * renderer;

    private:
      std::pair<float, float> map (int x, int y);
      DartEvent dart (float x, float y);
      void render ();

    public:
      Virtual ();
      void click (const SDL_MouseButtonEvent &);

    private:
      static const uint8_t VALUES [];
  };

}

#endif // __ED900_BOARD_VIRTUAL__

