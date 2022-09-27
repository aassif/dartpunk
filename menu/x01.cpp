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

  void X01::render () const
  {
    static const SDL_Rect R0 {32-2*8/2, 32-3*9/2+1*9, 16+1, 9};
    static const SDL_Rect R1 {32-2*8/2, 32-3*9/2+2*9, 16+1, 9};
    app->draw ("IN", 32-2*8/2+1, 32-3*9/2+0*9+1, 1);
    if (double_in)
    {
      app->draw ("1x", R0.x, R0.y+1, 1);
      app->draw (R0, App::Color (0), SDL_BLENDMODE_MOD);
      app->draw (R1, App::Color (0), SDL_BLENDMODE_NONE);
      app->draw ("2x", R1.x, R1.y+1, 1);
    }
    else
    {
      app->draw (R0, App::Color (0), SDL_BLENDMODE_NONE);
      app->draw ("1x", R0.x, R0.y+1, 1);
      app->draw ("2x", R1.x, R1.y+1, 1);
      app->draw (R1, App::Color (0), SDL_BLENDMODE_MOD);
    }

    static const SDL_Rect R2 {96-2*8/2, 32-3*9/2+1*9, 16+1, 9};
    static const SDL_Rect R3 {96-2*8/2, 32-3*9/2+2*9, 16+1, 9};
    app->draw ("OUT", 96-3*8/2+1, 32-3*9/2+0*9+1, 1);
    if (double_out)
    {
      app->draw ("1x", R2.x, R2.y+1, 1);
      app->draw (R2, App::Color (1), SDL_BLENDMODE_MOD);
      app->draw (R3, App::Color (1), SDL_BLENDMODE_NONE);
      app->draw ("2x", R3.x, R3.y+1, 1);
    }
    else
    {
      app->draw (R2, App::Color (1), SDL_BLENDMODE_NONE);
      app->draw ("1x", R2.x, R2.y+1, 1);
      app->draw ("2x", R3.x, R3.y+1, 1);
      app->draw (R3, App::Color (1), SDL_BLENDMODE_MOD);
    }
  }

} // ed900::menu

