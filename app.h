#ifndef __DARTPUNK_APP__
#define __DARTPUNK_APP__

#include <string>

#include "font.h"
#include "bluetooth.h"
#include "settings.h"
#include "game/game.h"

#define WIDTH 128
#define HEIGHT 64

namespace dartpunk
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

    private:
      // Buttons.
      void button1 (bool special);
      void button2 (bool special);
      void button3 ();
      void button4 ();
      void button5 (bool special);
      // Settings.
      void settings_confirm ();
      // Game logic.
      void game_next ();
      void game_stop ();
      // Special behaviour.
      void exit (int);
      void cancel ();
      void confirm ();
      // Board events.
      void board_connection (board::EventPtr);
      void board_button     (board::EventPtr);
      void board_dart       (board::EventPtr);

    public:
      App ();
      ~App ();
      // Draw "text" at position p (relative to rectangle r).
      void draw (const std::string &,               const Point &, uint8_t font, const Blender & = blend::ALPHA);
      void draw (const std::string &, const Rect &, const Point &, uint8_t font, const Blender & = blend::ALPHA);
      // Draw "text" at position p (relative to rectangle r, highlighted if selected).
      void draw (const std::string &, const Rect &, const Point &, uint8_t font, const Color &, bool selected);
      // Rectangle.
      void draw (const Rect &, const Color &, const Blender & = blend::NONE);
      // Image.
      void draw (const Image &, const Rect & src, const Point & dst, const Blender & = blend::ALPHA);
      int run ();

    public:
      static const Color COLORS [];
  };
}

#endif // __DARTPUNK_APP__

