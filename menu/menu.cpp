#include <numeric>

#include "../app.h"
#include "menu.h"

using namespace std;

namespace ed900::menu
{

////////////////////////////////////////////////////////////////////////////////

  Abstract::Abstract (App * a) :
    app {a},
    selection {0}
  {
  }

  void Abstract::select (uint8_t k)
  {
    selection = k;
  }

  bool Abstract::confirm ()
  {
    return selection != 0;
  }

  bool Abstract::cancel ()
  {
    return true;
  }

////////////////////////////////////////////////////////////////////////////////

  Menu::Menu (App * app, const vector<string> & items) :
    Abstract {app},
    items {items}
  {
  }

  Menu::~Menu ()
  {
  }

  uint8_t Menu::index ()
  {
    return selection;
  }

  void Menu::render () const
  {
    size_t n = min<size_t> (4, items.size ());

    auto f = [] (size_t m, const string & item) {return max (m, item.size ());};
    size_t m = accumulate (items.begin (), items.begin () + n, 0, f);

    for (uint8_t i = 0; i < n; ++i)
    {
      size_t w = items[i].size ();
      SDL_Rect R = {64 - (2 + 4*w), 2 + 16*i, 4 + 8*w, 4 + 8};

      if (i+1 == selection)
        app->draw (R, App::Color (i), SDL_BLENDMODE_NONE);

      app->draw (items[i], R.x + 3, R.y + 2, 1);

      if (i+1 != selection)
        app->draw (R, App::Color (i), SDL_BLENDMODE_MOD);
    }
  }

} // ed900::menu

