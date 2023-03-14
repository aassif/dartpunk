#include <thread>
#include <iostream>
#include <signal.h>

#include "app.h"
#include "video.h"

#if DARTPUNK_BOARD_ED900
  #include "board/ed900.h"
#elif DARTPUNK_BOARD_VIRTUAL
  #include "board/virtual.h"
#else
  #error DARTPUNK_BOARD
#endif

#if DARTPUNK_INPUT_JOYSTICK
  #include "input/joystick.h"
#elif DARTPUNK_INPUT_KEYBOARD
  #include "input/keyboard.h"
#elif DARTPUNK_INPUT_VIRTUAL
  //
#else
  #error DARTPUNK_INPUT
#endif

#if DARTPUNK_DISPLAY_MATRIX
  #include "display/matrix.h"
#elif DARTPUNK_DISPLAY_VIRTUAL
  #include "display/virtual.h"
#else
  #error DARTPUNK_DISPLAY
#endif

#define LONG_PRESS(dt)      dt >= 1000ms
#define VERY_LONG_PRESS(dt) dt >= 5000ms

using namespace std;
using namespace chrono;

namespace dartpunk
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

#if DARTPUNK_SDL2
    SDL_Init (SDL_INIT_EVERYTHING);
#endif
  }

  App::~App ()
  {
    delete game;
    fonts.clear ();

#if DARTPUNK_SDL2
    SDL_Quit ();
#endif
  }

  void App::settings_confirm ()
  {
    game::Game * g = settings.confirm ();
    if (g)
    {
      game = g;
      state = State::GAME;
    }
  }

  void App::game_next ()
  {
    if (game->button (board::Button::NEXT))
      game_stop ();
  }

  void App::game_stop ()
  {
    delete game;
    game = nullptr;
    state = State::SETTINGS;
  }

  void App::input_next_very_long ()
  {
    stopped = 1;
  }

  void App::input_next_long ()
  {
    switch (state)
    {
      case State::SETTINGS:
        settings.cancel ();
        break;

      case State::GAME:
        game_stop ();
        break;
    }
  }

  void App::input_next ()
  {
    switch (state)
    {
      case State::SETTINGS:
        settings_confirm ();
        break;

      case State::GAME:
        game_next ();
        break;
    }
  }

  void App::board_connection (board::EventPtr e)
  {
    auto c = dynamic_pointer_cast<board::ConnectionEvent> (e);
    state = c->connected ? (game ? State::GAME : State::SETTINGS) : State::BLUETOOTH;
  }

  void App::board_dart (board::EventPtr e)
  {
    auto d = dynamic_pointer_cast<board::DartEvent> (e);
    cout << d->text (true) << endl;
    if (game) game->dart (*d);
  }

  void App::board_button (board::EventPtr e)
  {
    auto b = dynamic_pointer_cast<board::ButtonEvent> (e);
    if (game && game->button (*b)) game_stop ();
  }

  void App::run ()
  {
#if DARTPUNK_BOARD_ED900
    board::ED900 board {"/org/bluez/hci0"};
#elif DARTPUNK_BOARD_VIRTUAL
    board::Virtual board;
#endif

#if DARTPUNK_INPUT_JOYSTICK
    input::Device input {"/dev/input/event0"};
#elif DARTPUNK_INPUT_KEYBOARD
    input::Device input {"/dev/input/event4"};
#endif

#if DARTPUNK_DISPLAY_MATRIX
    display::Matrix display;
#elif DARTPUNK_DISPLAY_VIRTUAL
    display::Virtual display {5};
#endif

    auto t0 = steady_clock::now ();
    for (auto t1 = t0 + 40ms; ! stopped; t1 += 40ms)
    {
#if DARTPUNK_SDL2
      SDL_Event e;
      while (SDL_PollEvent (&e))
      {
        switch (e.type)
        {
#if DARTPUNK_BOARD_VIRTUAL
          case SDL_MOUSEBUTTONUP:
            board.click (e.button);
            break;
#endif

#if DARTPUNK_INPUT_VIRTUAL
          case SDL_KEYDOWN:
            if (! e.key.repeat)
              t0 = steady_clock::time_point {milliseconds {e.key.timestamp}};
            break;

          case SDL_KEYUP:
          {
            auto dt = steady_clock::time_point {milliseconds {e.key.timestamp}} - t0;
            switch (e.key.keysym.sym)
            {
              case SDLK_ESCAPE:
                stopped = 1;
                break;

              case SDLK_1: settings.select (1); break;
              case SDLK_2: settings.select (2); break;
              case SDLK_3: settings.select (3); break;
              case SDLK_4: settings.select (4); break;

              case SDLK_SPACE:
                if (VERY_LONG_PRESS (dt))
                  input_next_very_long ();
                else if (LONG_PRESS (dt))
                  input_next_long ();
                else
                  input_next ();
            }
            break;
          }
#endif
        }
      }
#endif

#if DARTPUNK_EVDEV
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
                //cout << "Button: " << (uint16_t) DARTPUNK_BUTTON (e) << endl;
                auto dt = t - t0;
                switch (e->code)
                {
#if DARTPUNK_INPUT_KEYBOARD
                  case KEY_ESC:
                    stopped = 1;
                    break;
#endif

                  case DARTPUNK_BUTTON_1: settings.select (1); break;
                  case DARTPUNK_BUTTON_2: settings.select (2); break;
                  case DARTPUNK_BUTTON_3: settings.select (3); break;
                  case DARTPUNK_BUTTON_4: settings.select (4); break;

                  case DARTPUNK_BUTTON_NEXT:
                    if (VERY_LONG_PRESS (dt))
                      input_next_very_long ();
                    else if (LONG_PRESS (dt))
                      input_next_long ();
                    else
                      input_next ();
                }
                break;
              }
            }
          }
        }
      }
#endif

      for (board::EventPtr e = board.poll (); e; e = board.poll ())
      {
        switch (e->type)
        {
          case board::EventType::CONNECTION: board_connection (e); break;
          case board::EventType::DART:       board_dart       (e); break;
          case board::EventType::BUTTON:     board_button     (e); break;
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

      this_thread::sleep_until (t1);
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

