#include <iostream> // debug
#include <SDL2/SDL.h>

#include "virtual.h"

#define WINDOW_RADIUS 300

#define BOARD_BULL_INNER     6.35
#define BOARD_BULL_OUTER    16.0
#define BOARD_TRIPLE_START  99.0
#define BOARD_TRIPLE_END   107.0
#define BOARD_DOUBLE_START 162.0
#define BOARD_DOUBLE_END   170.0
#define BOARD_RADIUS       225.5

using namespace std; // debug

namespace ed900::board
{

  Virtual::Virtual () :
    Board {},
    window {SDL_CreateWindow ("ED900 - Board", 0, 0, 2*WINDOW_RADIUS, 2*WINDOW_RADIUS, 0)},
    renderer {SDL_CreateRenderer (window, -1, 0)}
  {
    emitConnectionEvent (true);
    render ();
  }

  pair<float, float> Virtual::map (int x, int y)
  {
    int w, h;
    SDL_GetWindowSize (window, &w, &h);
    float r = 0.5 * min (w, h);
    return pair<float, float> (BOARD_DOUBLE_END * (x - 0.5 * w) / r, BOARD_DOUBLE_END * (y - 0.5 * h) / r);
  };

  DartEvent Virtual::dart (float x, float y)
  {
    float l = sqrt (x*x + y*y);

    if (l <= BOARD_BULL_INNER) return DartEvent (25, 2);
    if (l <= BOARD_BULL_OUTER) return DartEvent (25, 1);
    if (l >  BOARD_DOUBLE_END) return DartEvent (0, 0);

    static const float K {2 * M_PI / 20};
    int k = floor (5.5 + atan2 (y, x) / K);

    uint8_t m = 1;
    if (l > BOARD_TRIPLE_START && l <= BOARD_TRIPLE_END) m = 3;
    if (l > BOARD_DOUBLE_START && l <= BOARD_DOUBLE_END) m = 2;

    int k20 = (k + 20) % 20;
    return DartEvent (VALUES [k20], m);
  }

  void Virtual::render ()
  {
    auto c = [] (const DartEvent & e) {
      static SDL_Color BLACK {0x00, 0x00, 0x00, 0xFF};
      static SDL_Color WHITE {0xFF, 0xFF, 0xFF, 0xFF};
      static SDL_Color GREEN {0x00, 0xFF, 0x00, 0xFF};
      static SDL_Color RED   {0xFF, 0x00, 0x00, 0xFF};
      static SDL_Color BLUE  {0x00, 0x00, 0xFF, 0xFF};

      switch (e.value)
      {
        case 25:
          switch (e.multiplier)
          {
            case 1: return GREEN;
            case 2: return RED;
          }
          return BLUE;

        case 2:
        case 3:
        case 7:
        case 8:
        case 10:
        case 12:
        case 13:
        case 14:
        case 18:
        case 20:
          switch (e.multiplier)
          {
            case 1: return BLACK;
            case 2: return RED;
            case 3: return RED;
          }
          return BLUE;

        case 1:
        case 4:
        case 5:
        case 6:
        case 9:
        case 11:
        case 15:
        case 16:
        case 17:
        case 19:
          switch (e.multiplier)
          {
            case 1: return WHITE;
            case 2: return GREEN;
            case 3: return GREEN;
          }
          return BLUE;

        default:
          return BLUE;
      }
    };

    int w, h;
    SDL_GetWindowSize (window, &w, &h);
    SDL_Color T [h][w];

    for (int row = 0; row < h; ++row)
      for (int col = 0; col < w; ++col)
      {
        auto [x, y] = map (row, col);
        DartEvent d = dart (x, y);
        T [row][col] = c (d);
      }

    SDL_Texture * texture = SDL_CreateTexture (renderer, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_STATIC, w, h);
    SDL_UpdateTexture (texture, NULL, &(T[0][0]), w * 4);
    SDL_RenderCopy (renderer, texture, NULL, NULL);
    SDL_DestroyTexture (texture);
    SDL_RenderPresent (renderer);
  }

  void Virtual::click (const SDL_MouseButtonEvent & b)
  {
    if (b.windowID == SDL_GetWindowID (window))
    {
      auto [x, y] = map (b.x, b.y);
      DartEvent d = dart (x, y);
      emitDartEvent (d.value, d.multiplier);
    }
  }

  const uint8_t Virtual::VALUES [] {
    20, 1, 18, 4, 13, 6, 10, 15, 2, 17, 3, 19, 7, 16, 8, 11, 14, 9, 12, 5
  };
}

