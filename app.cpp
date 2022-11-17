#include <iostream>

#include "app.h"
#include "input.h"
#include "ed900.h"

#define LONG_PRESS(dt)      dt >= 1000ms
#define VERY_LONG_PRESS(dt) dt >= 5000ms

#ifdef ED900_RGB_MATRIX
  #include "led-matrix.h"
  using namespace rgb_matrix;
#else
  #include <SDL2/SDL.h>
#endif

#ifdef ED900_RGB_MATRIX
  #define ED900_BUTTON_1     BTN_TRIGGER
  #define ED900_BUTTON_2     BTN_THUMB
  #define ED900_BUTTON_3     BTN_THUMB2
  #define ED900_BUTTON_4     BTN_TOP
  #define ED900_BUTTON_OK    BTN_BASE4
#else
  #define ED900_BUTTON_1     KEY_1
  #define ED900_BUTTON_2     KEY_2
  #define ED900_BUTTON_3     KEY_3
  #define ED900_BUTTON_4     KEY_4
  #define ED900_BUTTON_OK    KEY_SPACE
#endif

using namespace std;
using namespace chrono;

namespace ed900
{

  volatile sig_atomic_t stopped = 0;

  App::App () :
    matrix {},
    fonts {},
    state {State::BLUETOOTH},
    bluetooth {this},
    settings {this},
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
  }

  App::~App ()
  {
    delete game;
    fonts.clear ();
  }

  void App::run ()
  {
    ED900 ed900;
    ed900.start ("/org/bluez/hci0");

#ifdef ED900_RGB_MATRIX
    Input input ("/dev/input/event0");
    RGBMatrix::Options options;
    options.hardware_mapping = "adafruit-hat";
    options.cols = 64;
    options.rows = 64;
    options.chain_length = 2;
    options.pwm_bits = 11;
    options.brightness = 100;
    //options.limit_refresh_rate_hz = 25;
    //options.show_refresh_rate = false;
    rgb_matrix::RuntimeOptions runtime;
    //runtime.daemon = 1;
    //runtime.gpio_slowdown = 0;
    RGBMatrix * m = RGBMatrix::CreateFromOptions (options, runtime);
    FrameCanvas * canvas = m->CreateFrameCanvas ();
#else
    Input input ("/dev/input/event4");
    SDL_Init (SDL_INIT_EVERYTHING);
    SDL_Window * window = SDL_CreateWindow ("ED900", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIDTH * SCALE, HEIGHT * SCALE, 0);
    SDL_Renderer * renderer = SDL_CreateRenderer (window, -1, SDL_RENDERER_SOFTWARE);
    SDL_RenderSetLogicalSize (renderer, WIDTH, HEIGHT);
    SDL_Texture * texture = SDL_CreateTexture (renderer, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_STREAMING, WIDTH, HEIGHT);
#endif

    auto t0 = steady_clock::now ();

    while (! stopped)
    {
      for (Input::EventPtr e = input.poll (); e; e = input.poll ())
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
#ifndef ED900_RGB_MATRIX
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
                          if (game->button (Button::NEXT))
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

      for (ED900::EventPtr e = ed900.poll (); e; e = ed900.poll ())
      {
        switch (e->type)
        {
          case EventType::CONNECTION:
          {
            auto c = dynamic_pointer_cast<ConnectionEvent> (e);
            if (c->connected)
              state = game ? State::GAME : State::SETTINGS;
            else
              state = State::BLUETOOTH;
            break;
          }

          case EventType::DART:
          {
            auto d = dynamic_pointer_cast<DartEvent> (e);
            if (game)
              game->dart (*d);
            break;
          }

          case EventType::BUTTON:
          {
            auto b = dynamic_pointer_cast<ButtonEvent> (e);
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
          bluetooth.render ();
          break;

        case State::SETTINGS :
          settings.render ();
          break;

        case State::GAME:
          game->render (this);
          break;
      }

#ifdef ED900_RGB_MATRIX
      for (uint8_t y = 0; y < HEIGHT; ++y)
        for (uint8_t x = 0; x < WIDTH; ++x)
        {
          const Color & c = matrix [y][x];
          canvas->SetPixel (x, y, c.r, c.g, c.b);
        }
      canvas = m->SwapOnVSync (canvas);
#else
      SDL_UpdateTexture (texture, NULL, &(matrix[0][0]), WIDTH * 4);
      SDL_RenderCopy (renderer, texture, NULL, NULL);
      SDL_RenderPresent (renderer);
#endif

      this_thread::sleep_for (20ms);
    }

#ifdef ED900_RGB_MATRIX
    delete m;
#else
    SDL_DestroyTexture (texture);
    SDL_DestroyRenderer (renderer);
    SDL_DestroyWindow (window);
    SDL_Quit ();
#endif

    ed900.stop ();
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

  void App::draw (const std::string & text, int x, int y, uint8_t font)
  {
    if (font < fonts.size ())
      fonts [font].draw (this, text, x, y);
  }

  void App::draw (const Image & image, const Rect & src, const Point & dst)
  {
    for (int y = 0, sy = src.y, dy = dst.y; y < src.h; ++y, ++sy, ++dy)
      for (int x = 0, sx = src.x, dx = dst.x; x < src.w; ++x, ++sx, ++dx)
        matrix [dy][dx] = blend::BLEND (image [sy][sx], matrix [dy][dx]);
  }

}

