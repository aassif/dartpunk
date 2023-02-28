#include <iostream> // debug
#include <fstream>
#include <algorithm>

#include "../app.h"
#include "players.h"

using namespace std;

namespace ed900::menu
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
    if (count > 0)
    {
      switch (k)
      {
        case 1: selection = find_prev (selection); break;
        case 2: selection = find_next (selection); break;
      }
    }
    else
      selection = k;
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

    for (uint8_t y = 0; y < 6; ++y)
      for (uint8_t x = 0; x < 4; ++x)
      {
        uint8_t n = 4*y + x;
        if (n < config.size ())
        {
          Rect r {32*x, 16+8*y, 32, 8};
          bool selected = (n == selection);

          if (selected)
            app->draw (r, color);

          static const Color WHITE {0xFF, 0xFF, 0xFF, 0xFF};
          Color c = states [n] != 0 ? App::COLORS [states [n] - 1] : WHITE;
          app->draw (config [n], r, {1, 1}, 0, blend::ModAlpha (c));
        }
      }
  }

  void Players::render (App * app) const
  {
    if (count == 0)
      render_count (app);
    else
      render_names (app);
  }

} // ed900::menu

