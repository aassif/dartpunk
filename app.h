#ifndef __ED900_APP__
#define __ED900_APP__

#include <string>

#include <SDL2/SDL.h>

#include "font.h"
#include "bluetooth.h"
#include "settings.h"
#include "game/game.h"

#define WIDTH 128
#define HEIGHT 64

#ifdef ED900_RGB_MATRIX
  #define SCALE 1
#else
  #define SCALE 5
#endif

namespace ed900
{
  class App
  {
    public:
      enum class State : uint8_t
      {
        BLUETOOTH = 0,
        SETTINGS = 1,
        GAME = 2
      };

    private:
      SDL_Renderer * renderer;
      std::vector<Font> fonts;
      State state;
      Bluetooth bluetooth;
      Settings settings;
      game::Game * game;

    public:
      App (SDL_Renderer *);
      ~App ();
      SDL_Texture * load (const std::string & path);
      void draw (const std::string &, int x, int y, uint8_t font);
      void draw (const SDL_Rect &, const SDL_Color &, SDL_BlendMode);
      void draw (const SDL_Texture *, const SDL_Rect &);
      void run ();

    private:
      static const uint8_t COLORS [][3];
    public:
      static SDL_Color Color (uint8_t k, float alpha = 1.0);
  };
}

#endif // __ED900_APP__

