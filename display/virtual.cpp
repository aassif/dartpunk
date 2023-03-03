#include "virtual.h"

namespace dartpunk::display
{

  Virtual::Virtual (uint8_t scale) :
    window {nullptr},
    renderer {nullptr},
    texture {nullptr}
  {
    window = SDL_CreateWindow ("DartPunk - Display", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIDTH * scale, HEIGHT * scale, 0);

    renderer = SDL_CreateRenderer (window, -1, SDL_RENDERER_SOFTWARE);
    SDL_RenderSetLogicalSize (renderer, WIDTH, HEIGHT);

    texture = SDL_CreateTexture (renderer, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_STREAMING, WIDTH, HEIGHT);
  }

  Virtual::~Virtual ()
  {
    SDL_DestroyTexture (texture);
    SDL_DestroyRenderer (renderer);
    SDL_DestroyWindow (window);
  }

  void Virtual::operator() (const App::Matrix & matrix)
  {
    SDL_UpdateTexture (texture, NULL, &(matrix[0][0]), WIDTH * 4);
    SDL_RenderCopy (renderer, texture, NULL, NULL);
    SDL_RenderPresent (renderer);
  }

} // dartpunk::display

