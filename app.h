#ifndef __ED900_APP__
#define __ED900_APP__

#include <string>

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

      typedef std::array<Color, WIDTH> MatrixRow;
      typedef std::array<MatrixRow, HEIGHT> Matrix;

    private:
      Matrix matrix;
      std::vector<Font> fonts;
      State state;
      Bluetooth bluetooth;
      Settings settings;
      game::Game * game;

    public:
      App ();
      ~App ();
      void draw (const std::string &, int x, int y, uint8_t font);
      void draw (const Rect &, const Color &, const Blender & = blend::NONE);
      void draw (const Image &, const Rect & src, const Point & dst);
      void run ();

    public:
      static const Color COLORS [];
  };
}

#endif // __ED900_APP__

