#include "../app.h"
#include "x01.h"

using namespace std;

namespace dartpunk::menu
{

  X01::X01 () :
    Abstract {},
    double_in {false},
    double_out {true}
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

  void X01::render (App * app) const
  {
    static auto R = [] (int x, int y, int k) {
      return Rect {32 + x*64 - k*8/2, 32 - 3*9/2 + y*9, 16 + 1, 9};
    };

    static const Point P {0, 1};

    app->draw ("IN",  R(0,0,2), P, 1);
    app->draw ("1x",  R(0,1,2), P, 1, App::COLORS [0], ! double_in);
    app->draw ("2x",  R(0,2,2), P, 1, App::COLORS [0],   double_in);

    app->draw ("OUT", R(1,0,3), P, 1);
    app->draw ("1x",  R(1,1,2), P, 1, App::COLORS [1], ! double_out);
    app->draw ("2x",  R(1,2,2), P, 1, App::COLORS [1],   double_out);
  }

} // dartpunk::menu

