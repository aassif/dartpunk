#include <numeric>

#include "../app.h"
#include "menu.h"

using namespace std;

namespace ed900::menu
{

////////////////////////////////////////////////////////////////////////////////

  Abstract::Abstract () :
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

  Menu::Menu (const vector<string> & items) :
    Abstract {},
    items {items}
  {
  }

  uint8_t Menu::index ()
  {
    return selection;
  }

  void Menu::render (App * app) const
  {
    size_t n = min<size_t> (4, items.size ());

    auto f = [] (size_t m, const string & item) {return max (m, item.size ());};
    size_t m = accumulate (items.begin (), items.begin () + n, 0, f);

    for (uint8_t i = 0; i < n; ++i)
    {
      uint8_t w = items[i].size ();
      Rect r {64 - (2 + 4*w), 2 + 16*i, 4 + 8*w, 4 + 8};
      const Color & c = App::COLORS [i];
      bool selected = (i+1 == selection);
      app->draw (items [i], r, {3, 2}, 1, c, selected);
    }
  }

} // ed900::menu

