#include <iostream> // debug
#include <fstream>
#include <algorithm>

#include "../app.h"
#include "players.h"

using namespace std;

#define ROWS 5
#define COLS 4
#define PAGE (ROWS*COLS)

namespace dartpunk::menu
{

  Players::Players () :
    Abstract {},
    icon {"images/player.png"},
    config {},
    count {0},
    states {},
    result {}
  {
    ifstream ifs ("names.txt");
    for (string line; getline (ifs, line);)
      config.push_back (line);

    static const vector<string> CONFIG {"P1", "P2", "P3", "P4"};
    if (config.size () < 4)
      copy (CONFIG.begin () + config.size (), CONFIG.end (), back_inserter (config));

    states.resize (config.size ());
  }

  uint8_t Players::find_prev (uint8_t k0)
  {
    for (uint8_t k = k0; k > 0; --k)
      if (states [k-1] == 0) return k-1;

    return k0;
  }

  uint8_t Players::find_next (uint8_t k0)
  {
    for (uint8_t k = k0; k < config.size () - 1; ++k)
      if (states [k+1] == 0) return k+1;

    return k0;
  }

  uint8_t Players::find_first ()
  {
    return states [0] == 0 ? 0 : find_next (0);
  }

  void Players::select (uint8_t k)
  {
    if (count == 0)
      selection = k;
    else
    {
      switch (k)
      {
        case 1: selection = find_prev (selection); break;
        case 2: selection = find_next (selection); break;
      }
    }
  }

  bool Players::confirm ()
  {
    if (count == 0)
    {
      count = selection;
      selection = 0;
      fill (states.begin (), states.end (), 0);
      result.clear ();
      return false;
    }
    else
    {
      if (selection < config.size ())
      {
        states [selection] = 1 + result.size ();
        result.push_back (selection);
        selection = find_first ();
        return count == result.size ();
      }
      return false;
    }
  }

  bool Players::cancel ()
  {
    if (count == 0)
      return true;
    else
    {
      if (! result.empty ())
      {
        uint8_t k = result.back ();
        result.pop_back ();
        states [k] = 0;
      }
      else
        count = 0;

      return false;
    }
  }

  Players::operator vector<string> () const
  {
    vector<string> v;
    auto f = [this] (uint8_t k) {return config [k];};
    transform (result.begin (), result.end (), back_inserter (v), f);
    return v;
  }

  void Players::render_count (App * app) const
  {
    for (uint8_t y = 0; y < 2; ++y)
      for (uint8_t x = 0; x < 2; ++x)
      {
        uint8_t n = 2*y + x + 1;

        Rect r {64*x, 32*y, 64, 32};
        const Color & c = App::COLORS [n-1];
        bool selected = (n == selection);

        if (selected)
          app->draw (r, c);

        Blender b = selected ? blend::ALPHA : blend::ModAlpha (c);

        for (uint8_t i = 0; i < n; ++i)
        {
          Point dst {r.x + 32 - 5*n + 10*i, r.y + 8};
          app->draw (icon, {0, 0, 9, 16}, dst, b);
        }
      }
  }

  void Players::render_names (App * app) const
  {
    string title = "Player " + to_string (1 + result.size ());
    const Color & color = App::COLORS [result.size ()];
    app->draw (title, {64 - 4 * title.size (), 2}, 1, blend::ModAlpha (color));

    uint8_t p = selection / PAGE;
    for (uint8_t y = 0; y < ROWS; ++y)
      for (uint8_t x = 0; x < COLS; ++x)
      {
        uint8_t k = PAGE*p + COLS*y + x;
        if (k < config.size ())
        {
          Rect r {32*x, 14+8*y, 32, 8};
          bool selected = (k == selection);

          if (selected)
            app->draw (r, color);

          static const Color WHITE {0xFF, 0xFF, 0xFF, 0xFF};
          Color c = states [k] != 0 ? App::COLORS [states [k] - 1] : WHITE;
          app->draw (config [k], r, {1, 1}, 0, blend::ModAlpha (c));
        }
      }

    app->draw ("PREV", {1+0,  64 - 6}, 0, blend::ModAlpha (App::COLORS[0]));
    app->draw ("NEXT", {1+32, 64 - 6}, 0, blend::ModAlpha (App::COLORS[1]));
    //app->draw ("NEW", {1+64, 64 - 6}, 0, blend::ModAlpha (App::COLORS[2]));

    uint8_t n = (config.size () + (PAGE-1)) / PAGE;
    if (n > 1)
    {
      string page = to_string (p+1) + "/" + to_string (n);
      app->draw (page, {128 - 4 * page.size (), 64 - 6}, 0);
    }
  }

/*
  void Players::render_keyboard (App * app)
  {
    static const uint8_t KEYBOARD =
    {
      {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', ' ', ' ', ' '},
      {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M'},
      {'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z'},
      {'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm'},
      {'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z'}
    };
  }
*/

  void Players::render (App * app) const
  {
    if (count == 0)
      render_count (app);
    else
      render_names (app);
  }

} // dartpunk::menu

