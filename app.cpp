#include <thread>
#include <iostream>
#include <signal.h>

#include "app.h"
#include "button.h"
#include "video.h"

#define DARTPUNK_QUIT     0
#define DARTPUNK_RESTART -1
#define DARTPUNK_DEFAULT  1

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
  #define DARTPUNK_BUTTON_1 SDLK_1
  #define DARTPUNK_BUTTON_2 SDLK_2
  #define DARTPUNK_BUTTON_3 SDLK_3
  #define DARTPUNK_BUTTON_4 SDLK_4
  #define DARTPUNK_BUTTON_5 SDLK_SPACE
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

using namespace std;
using namespace chrono;

namespace dartpunk
{

  volatile sig_atomic_t status = DARTPUNK_DEFAULT;

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

    auto f = [] (int s) {
      switch (s)
      {
        case SIGINT:
        case SIGTERM:
          status = DARTPUNK_QUIT;
          break;

        default:
          status = DARTPUNK_RESTART;
          break;
      }
    };

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

  void App::button1 (bool special)
  {
    if (special)
      exit (DARTPUNK_QUIT);
    else
      settings.select (1);
  }

  void App::button2 (bool special)
  {
    if (special)
      exit (DARTPUNK_RESTART);
    else
      settings.select (2);
  }

  void App::button3 ()
  {
    settings.select (3);
  }

  void App::button4 ()
  {
    settings.select (4);
  }

  void App::button5 (bool special)
  {
    if (special)
      cancel ();
    else
      confirm ();
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

  void App::exit (int s)
  {
    status = s;
  }

  void App::cancel ()
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

  void App::confirm ()
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

  int App::run ()
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

    Button b1 {5s};
    Button b2 {3s};
    Button b5 {1s};

    for (TimePoint t1 {steady_clock::now () + 40ms}; status > 0; t1 += 40ms)
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
          {
            TimePoint t {milliseconds {e.key.timestamp}};
            if (! e.key.repeat)
            {
              switch (e.key.keysym.sym)
              {
                case SDLK_1:     b1 = t; break;
                case SDLK_2:     b2 = t; break;
                case SDLK_SPACE: b5 = t; break;
              }
            }
            break;
          }

          case SDL_KEYUP:
          {
            TimePoint t {milliseconds {e.key.timestamp}};
            switch (e.key.keysym.sym)
            {
              case SDLK_ESCAPE:
                exit (DARTPUNK_QUIT);
                break;

              case SDLK_BACKSPACE:
                cancel ();
                break;

              case DARTPUNK_BUTTON_1: button1 (b1 [t]); break;
              case DARTPUNK_BUTTON_2: button2 (b2 [t]); break;
              case DARTPUNK_BUTTON_3: button3 (/****/); break;
              case DARTPUNK_BUTTON_4: button4 (/****/); break;
              case DARTPUNK_BUTTON_5: button5 (b5 [t]); break;
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
            auto t = e->time_point ();

#if 0
            cout << libevdev_event_type_get_name (e->type) << " : "
                 << libevdev_event_code_get_name (e->type, e->code) << " : "
                 << e->value << endl;
#endif

            switch (e->value)
            {
              // Down.
              case 1:
                switch (e->code)
                {
                  case DARTPUNK_BUTTON_1: b1 = t; break;
                  case DARTPUNK_BUTTON_2: b2 = t; break;
                  case DARTPUNK_BUTTON_5: b5 = t; break;
                }
                break;

              // Repeat.
              case 2:
                break;

              // Up.
              case 0:
              {
                //cout << "Button: " << (uint16_t) DARTPUNK_BUTTON (e) << endl;
                switch (e->code)
                {
#if DARTPUNK_INPUT_KEYBOARD
                  case KEY_ESC:
                    exit (DARTPUNK_QUIT);
                    break;

                  case KEY_BACKSPACE:
                    cancel ();
                    break;
#endif

                  case DARTPUNK_BUTTON_1: button1 (b1 [t]); break;
                  case DARTPUNK_BUTTON_2: button2 (b2 [t]); break;
                  case DARTPUNK_BUTTON_3: button3 (/****/); break;
                  case DARTPUNK_BUTTON_4: button4 (/****/); break;
                  case DARTPUNK_BUTTON_5: button5 (b5 [t]); break;
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

      static Video v1 {"dawson.png", 64};
      static Video v2 {"reset.png", 16, 2};

#if DARTPUNK_INPUT_VIRTUAL
      TimePoint t {milliseconds {SDL_GetTicks ()}};
#else
      TimePoint t {input::now ()};
#endif

      v1.render (this, b1 (t));
      v2.render (this, b2 (t));

      display (matrix);

      this_thread::sleep_until (t1);
    }

    return status;
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

