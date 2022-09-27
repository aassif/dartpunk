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
    fonts {},
    state {State::BLUETOOTH},
    bluetooth {this},
    settings {this},
    game {nullptr}
  {
    fonts.emplace_back (FontTomThumb {renderer});
    fonts.emplace_back (FontTopaz {renderer});
    fonts.emplace_back (FontScore {renderer});
    fonts.emplace_back (FontRoboto {renderer});
    fonts.emplace_back (FontCricket {renderer});
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
    RGBMatrix * matrix = RGBMatrix::CreateFromOptions (options, runtime);
    FrameCanvas * canvas = matrix->CreateFrameCanvas ();
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

      SDL_SetRenderDrawColor (renderer, 0, 0, 0, 0);
      SDL_RenderClear (renderer);
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
      uint8_t pixels [WIDTH * HEIGHT * 4];
      SDL_RenderReadPixels (renderer, NULL, SDL_PIXELFORMAT_RGBA32, pixels, WIDTH * 4);
      for (uint8_t y = 0; y < HEIGHT; ++y)
        for (uint8_t x = 0; x < WIDTH; ++x)
        {
          uint8_t * p = pixels + (y * WIDTH + x) * 4;
          canvas->SetPixel (x, y, p[0], p[1], p[2]);
        }
      canvas = matrix->SwapOnVSync (canvas);
      SDL_Delay (40);
#endif

      SDL_RenderPresent (renderer);
    }

    ed900.stop ();
  }

  const uint8_t App::COLORS [4][3] =
  {
    {0xEE, 0x00, 0x00}, // R // 157 0 0
    {0x00, 0x00, 0xFF}, // B // 0 0 255
    {0xCC, 0xCC, 0x00}, // Y // 91 91 0
    {0x00, 0xDD, 0x00}  // G // 0 112 0
  };

  SDL_Color App::Color (uint8_t k, float alpha)
  {
    const auto & c = COLORS [k];
    return SDL_Color {c[0], c[1], c[2], static_cast<uint8_t> (round (255 * alpha))};
  }

  SDL_Texture * App::load (const string & path)
  {
    return IMG_LoadTexture (renderer, path.c_str ());
  }

  void App::draw (const SDL_Rect & r, const SDL_Color & c, SDL_BlendMode mode)
  {
    SDL_SetRenderDrawBlendMode (renderer, mode);
    SDL_SetRenderDrawColor (renderer, c.r, c.g, c.b, c.a);
    SDL_RenderFillRect (renderer, &r);
  }

  void App::draw (const std::string & text, int x, int y, uint8_t font)
  {
    if (font < fonts.size ())
      fonts [font].draw (text, x, y);
  }

  void App::draw (const SDL_Texture * t, const SDL_Rect & r)
  {
    SDL_RenderCopy (renderer, (SDL_Texture *) t, NULL, &r);
  }
}

