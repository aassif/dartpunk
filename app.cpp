#include <iostream>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "app.h"
#include "ed900.h"

#define LONG_PRESS(dt)      dt >= 1000
#define VERY_LONG_PRESS(dt) dt >= 5000

#ifdef ED900_RGB_MATRIX
  #include "led-matrix.h"
  using namespace rgb_matrix;
#endif

#ifdef ED900_RGB_MATRIX
  #define ED900_REPEAT(e)    false
  #define ED900_TIMESTAMP(e) e.jbutton.timestamp
  #define ED900_BUTTON(e)    e.jbutton.button
  #define ED900_BUTTONUP     SDL_JOYBUTTONUP
  #define ED900_BUTTONDOWN   SDL_JOYBUTTONDOWN
  #define ED900_BUTTON_1     0
  #define ED900_BUTTON_2     1
  #define ED900_BUTTON_3     2
  #define ED900_BUTTON_4     3
  #define ED900_BUTTON_OK    9
#else
  #define ED900_REPEAT(e)    e.key.repeat
  #define ED900_TIMESTAMP(e) e.key.timestamp
  #define ED900_BUTTON(e)    e.key.keysym.sym
  #define ED900_BUTTONUP     SDL_KEYUP
  #define ED900_BUTTONDOWN   SDL_KEYDOWN
  #define ED900_BUTTON_1     SDLK_1
  #define ED900_BUTTON_2     SDLK_2
  #define ED900_BUTTON_3     SDLK_3
  #define ED900_BUTTON_4     SDLK_4
  #define ED900_BUTTON_OK    SDLK_SPACE
#endif

using namespace std;

namespace ed900
{
  App::App (SDL_Renderer * r) :
    renderer {r},
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
    SDL_Texture * texture = SDL_CreateTexture (renderer, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_STREAMING, WIDTH, HEIGHT);
#endif

    auto t0 = SDL_GetTicks ();

    bool stop = false;
    while (! stop)
    {
      SDL_Event e;
      while (SDL_PollEvent (&e))
      {
        switch (e.type)
        {
          case SDL_QUIT:
            stop = true;
            break;

          case ED900_BUTTONDOWN:
            if (! ED900_REPEAT (e))
              t0 = ED900_TIMESTAMP (e);
            break;

          case ED900_BUTTONUP:
            {
              //cout << "Button: " << (uint16_t) ED900_BUTTON (e) << endl;
              auto dt = ED900_TIMESTAMP (e) - t0;
              switch (ED900_BUTTON (e))
              {
#ifndef ED900_RGB_MATRIX
                case SDLK_ESCAPE:
                  stop = true;
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
                        stop = true;
                      break;

                    case State::SETTINGS:
                      if (VERY_LONG_PRESS (dt))
                        stop = true;
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
                        stop = true;
                      else if (LONG_PRESS (dt))
                      {
                        delete game;
                        state = State::SETTINGS;
                      }
                      else
                        game->button (Button::NEXT);
                      break;
                  }
                  break;
              }
            }
            break;
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
    }

#ifdef ED900_RGB_MATRIX
#else
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

