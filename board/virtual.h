#ifndef __DARTPUNK_BOARD_VIRTUAL__
#define __DARTPUNK_BOARD_VIRTUAL__

#include <SDL2/SDL.h>

#include "board.h"

namespace dartpunk::board
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

} // dartpunk::board

#endif // __DARTPUNK_BOARD_VIRTUAL__

