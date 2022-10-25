#include <SDL2/SDL.h>

#include "app.h"

int main (int argc, char ** argv)
{
  SDL_Init (SDL_INIT_EVERYTHING);

  SDL_Window * w = SDL_CreateWindow ("ED900", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIDTH * SCALE, HEIGHT * SCALE, 0);

  SDL_Renderer * r = SDL_CreateRenderer (w, -1, SDL_RENDERER_SOFTWARE);
  SDL_RenderSetLogicalSize (r, WIDTH, HEIGHT);

  if (SDL_NumJoysticks () > 0)
    SDL_JoystickOpen (0);

  ed900::App app {r};
  app.run ();

  SDL_DestroyRenderer (r);
  SDL_DestroyWindow (w);
  SDL_Quit ();

  return 0;
}

