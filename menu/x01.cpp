#include "../app.h"
#include "x01.h"

using namespace std;

namespace ed900::menu
{

  X01::X01 (App * app) :
    Abstract {app},
    double_in {false},
    double_out {true}
  {
  }

  X01::~X01 ()
  {
  }

  void X01::select (uint8_t k)
  {
    switch (k)
    {
      case 1: double_in  = ! double_in;  break;
      case 2: double_out = ! double_out; break;
    }
  }

  bool X01::confirm ()
  {
    return true;
  }

  bool X01::cancel ()
  {
    return true;
  }

  X01::operator pair<bool, bool> () const
  {
    return {double_in, double_out};
  }

  void X01::render_text (const string & text, const Rect & r, const Color & c, bool selected) const
  {
    if (selected)
    {
      app->draw (r, c, blend::NONE);
      app->draw (text, r.x, r.y + 1, 1);
    }
    else
    {
      app->draw (text, r.x, r.y + 1, 1);
      app->draw (r, c, blend::MODULATE);
    }
  }

  void X01::render () const
  {
    static const Rect R0 {32-2*8/2, 32-3*9/2+1*9, 16+1, 9};
    static const Rect R1 {32-2*8/2, 32-3*9/2+2*9, 16+1, 9};

    app->draw ("IN", 32-2*8/2+1, 32-3*9/2+0*9+1, 1);
    render_text ("1x", R0, App::COLORS [0], ! double_in);
    render_text ("2x", R1, App::COLORS [0],   double_in);

    static const Rect R2 {96-2*8/2, 32-3*9/2+1*9, 16+1, 9};
    static const Rect R3 {96-2*8/2, 32-3*9/2+2*9, 16+1, 9};

    app->draw ("OUT", 96-3*8/2+1, 32-3*9/2+0*9+1, 1);
    render_text ("1x", R2, App::COLORS [1], ! double_out);
    render_text ("2x", R3, App::COLORS [1],   double_out);
  }

} // ed900::menu

