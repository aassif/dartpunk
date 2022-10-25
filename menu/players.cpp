#include <iostream> // debug
#include <fstream>
#include <algorithm>

#include "../app.h"
#include "players.h"

using namespace std;

namespace ed900::menu
{

  Players::Players (App * app) :
    Abstract {app},
    icon {"images/player.png"},
    config {"P1", "P2", "P3", "P4"},
    count {0},
    states {},
    result {}
  {
    ifstream ifs ("names.txt");
    for (string line; getline (ifs, line);)
      config.push_back (line);

    states.resize (config.size ());
  }

  uint8_t Players::find_prev (uint8_t k0)
  {
    if (k0 > 0)
    {
      for (uint8_t k = k0 - 1; k >= 0; --k)
        if (states [k] == 0) return k;
    }

    return k0;
  }

  uint8_t Players::find_next (uint8_t k0)
  {
    if (k0 + 1 < config.size ())
    {
      for (uint8_t k = k0 + 1; k < config.size (); ++k)
        if (states [k] == 0) return k;
    }

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

  void Players::render_count () const
  {
    for (uint8_t y = 0; y < 2; ++y)
      for (uint8_t x = 0; x < 2; ++x)
      {
        uint8_t n = 2*y + x + 1;
        bool active = (n == selection);

        Rect R {64*x, 32*y, 64, 32};

        if (active)
          app->draw (R, App::COLORS [n-1], blend::NONE);

        for (uint8_t i = 0; i < n; ++i)
        {
          Point dst {R.x + 32 - 5*n + 10*i, R.y + 8};
          app->draw (icon, {0, 0, 9, 16}, dst);
        }

        if (! active)
          app->draw (R, App::COLORS [n-1], blend::MODULATE);
      }
  }

  void Players::render_names () const
  {
    string title = "Player" + to_string (1 + result.size ());
    app->draw (title, 64-7*4, 2, 1);

    const Color & color = App::COLORS [result.size ()];
    app->draw ({64-7*4, 2, 7*8, 10}, color, blend::MODULATE);

    for (uint8_t y = 0; y < 6; ++y)
      for (uint8_t x = 0; x < 4; ++x)
      {
        uint8_t n = 4*y + x;
        bool active = (n == selection);

        Rect R {32*x, 16+8*y, 32, 8};

        if (active)
          app->draw (R, color, blend::NONE);

        if (n < config.size ())
          app->draw (config [n], R.x + 1, R.y + 1, 0);

        if (states[n] != 0)
          app->draw (R, App::COLORS [states[n]-1], blend::MODULATE);
      }
  }

  void Players::render () const
  {
    if (count == 0)
      render_count ();
    else
      render_names ();
  }

} // ed900::menu

