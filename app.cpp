#include <thread>
#include <iostream>
#include <signal.h>

#include "app.h"

#if ED900_BOARD_ED900
  #include "board/ed900.h"
#elif ED900_BOARD_VIRTUAL
  #include "board/virtual.h"
#else
  #error ED900_BOARD
#endif

#if ED900_INPUT_JOYSTICK
  #include "input/joystick.h"
#elif ED900_INPUT_KEYBOARD
  #include "input/keyboard.h"
#elif ED900_INPUT_VIRTUAL
  #include "input/virtual.h"
#else
  #error ED900_INPUT
#endif

#if ED900_DISPLAY_MATRIX
  #include "display/matrix.h"
#elif ED900_DISPLAY_VIRTUAL
  #include "display/virtual.h"
#endif

#define LONG_PRESS(dt)      dt >= 1000ms
#define VERY_LONG_PRESS(dt) dt >= 5000ms

using namespace std;
using namespace chrono;

namespace ed900
{

  volatile sig_atomic_t stopped = 0;

  App::App () :
    matrix {},
    fonts {},
    state {State::BLUETOOTH},
    bluetooth {},
    settings {},
    game {nullptr}
  {
    fonts.emplace_back (FontTomThumb {});
    fonts.emplace_back (FontTopaz {});
    fonts.emplace_back (FontScore {});
    fonts.emplace_back (FontRoboto {});
    fonts.emplace_back (FontCricket {});

    auto f = [] (int) {stopped = 1;};
    signal (SIGINT,  f);
    signal (SIGTERM, f);

#if ED900_SDL2
    SDL_Init (SDL_INIT_EVERYTHING);
#endif
  }

  App::~App ()
  {
    delete game;
    fonts.clear ();

#if ED900_SDL2
    SDL_Quit ();
#endif
  }

  void App::run ()
  {
#if ED900_BOARD_ED900
    board::ED900 board {"/org/bluez/hci0"};
#elif ED900_BOARD_VIRTUAL
    board::Virtual board;
#endif

#if ED900_DISPLAY_MATRIX
    display::Matrix display;
#elif ED900_DISPLAY_VIRTUAL
    display::Virtual display {5};
#endif

#if ED900_INPUT_JOYSTICK
    input::Device input {"/dev/input/event0"};
#elif ED900_INPUT_KEYBOARD
    input::Device input {"/dev/input/event4"};
#endif

    auto t0 = steady_clock::now ();

    while (! stopped)
    {
#if ED900_BOARD_VIRTUAL
      SDL_Event e;
      while (SDL_PollEvent (&e))
        if (e.type == SDL_MOUSEBUTTONUP)
          board.click (e.button);
#endif

      for (input::EventPtr e = input.poll (); e; e = input.poll ())
      {
        switch (e->type)
        {
          case EV_KEY:
          {
            auto t = steady_clock::time_point {seconds {e->input_event_sec} + microseconds {e->input_event_usec}};

#if 0
            cout << libevdev_event_type_get_name (e->type) << " : "
                 << libevdev_event_code_get_name (e->type, e->code) << " : "
                 << e->value << endl;
#endif

            switch (e->value)
            {
              // Down.
              case 1:
                t0 = t;
                break;

              // Repeat.
              case 2:
                break;

              // Up.
              case 0:
              {
                //cout << "Button: " << (uint16_t) ED900_BUTTON (e) << endl;
                auto dt = t - t0;
                switch (e->code)
                {
#if ED900_INPUT_KEYBOARD
                  case KEY_ESC:
                    stopped = 1;
                    break;
#endif

                  case ED900_BUTTON_1: settings.select (1); break;
                  case ED900_BUTTON_2: settings.select (2); break;
                  case ED900_BUTTON_3: settings.select (3); break;
                  case ED900_BUTTON_4: settings.select (4); break;

                  case ED900_BUTTON_OK:
                    switch (state)
                    {
                      case State::BLUETOOTH:
                        if (VERY_LONG_PRESS (dt))
                          stopped = 1;
                        break;

                      case State::SETTINGS:
                        if (VERY_LONG_PRESS (dt))
                          stopped = 1;
                        else if (LONG_PRESS (dt))
                          settings.cancel ();
                        else
                        {
                          game::Game * g = settings.confirm ();
                          if (g)
                          {
                            game = g;
                            state = State::GAME;
                          }
                        }
                        break;

                      case State::GAME:
                        if (VERY_LONG_PRESS (dt))
                          stopped = 1;
                        else if (LONG_PRESS (dt))
                        {
                          delete game;
                          state = State::SETTINGS;
                        }
                        else
                        {
                          if (game->button (board::Button::NEXT))
                          {
                            delete game;
                            game = nullptr;
                            state = State::SETTINGS;
                          }
                        }
                        break;
                    }
                    break;
                }
                break;
              }
            }
          }
        }
      }

      for (board::EventPtr e = board.poll (); e; e = board.poll ())
      {
        switch (e->type)
        {
          case board::EventType::CONNECTION:
          {
            auto c = dynamic_pointer_cast<board::ConnectionEvent> (e);
            if (c->connected)
              state = game ? State::GAME : State::SETTINGS;
            else
              state = State::BLUETOOTH;
            break;
          }

          case board::EventType::DART:
          {
            auto d = dynamic_pointer_cast<board::DartEvent> (e);
            cout << d->text (true) << endl;
            if (game)
              game->dart (*d);
            break;
          }

          case board::EventType::BUTTON:
          {
            auto b = dynamic_pointer_cast<board::ButtonEvent> (e);
            if (game && game->button (*b))
            {
              delete game;
              game = nullptr;
              state = State::SETTINGS;
            }
            break;
          }
        }
      }

      memset (&(matrix[0][0]), 0, 4 * WIDTH * HEIGHT);

      switch (state)
      {
        case State::BLUETOOTH :
          bluetooth.render (this);
          break;

        case State::SETTINGS :
          settings.render (this);
          break;

        case State::GAME:
          game->render (this);
          break;
      }

      display (matrix);

      this_thread::sleep_for (40ms);
    }
  }

  const Color App::COLORS [] =
  {
    {0xEE, 0x00, 0x00, 0xFF}, // R // 157 0 0
    {0x00, 0x00, 0xFF, 0xFF}, // B // 0 0 255
    {0xCC, 0xCC, 0x00, 0xFF}, // Y // 91 91 0
    {0x00, 0xDD, 0x00, 0xFF}  // G // 0 112 0
  };

  void App::draw (const Rect & r, const Color & c, const Blender & b)
  {
    for (int y = r.y; y < r.y + r.h; ++y)
      for (int x = r.x; x < r.x + r.w; ++x)
        matrix [y][x] = b (c, matrix [y][x]);
  }

  void App::draw (const string & text, const Point & p, uint8_t font, const Blender & b)
  {
    if (font < fonts.size ())
      fonts [font].draw (this, text, p, b);
  }

  void App::draw (const string & text, const Rect & r, const Point & p, uint8_t font, const Blender & b)
  {
    draw (text, {r.x + p.x, r.y + p.y}, font, b);
  }

  void App::draw (const string & text, const Rect & r, const Point & p, uint8_t font, const Color & c, bool selected)
  {
    if (selected)
    {
      draw (r, c);
      draw (text, r, p, font);
    }
    else
    {
      Blender b = blend::ModAlpha (c);
      draw (text, r, p, font, b);
    }
  }

  void App::draw (const Image & image, const Rect & src, const Point & dst, const Blender & b)
  {
    for (int y = 0, sy = src.y, dy = dst.y; y < src.h; ++y, ++sy, ++dy)
      for (int x = 0, sx = src.x, dx = dst.x; x < src.w; ++x, ++sx, ++dx)
        matrix [dy][dx] = b (image [sy][sx], matrix [dy][dx]);
  }

}

