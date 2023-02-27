#ifndef __ED900_MENU_X01__
#define __ED900_MENU_X01__

#include "menu.h"

namespace ed900::menu
{

  class X01 : public Abstract
  {
    private:
      bool double_in;
      bool double_out;

    public:
      X01 (App *);
      ~X01 ();
      virtual void select (uint8_t k);
      virtual bool confirm ();
      virtual bool cancel ();
      operator std::pair<bool, bool> () const;
      virtual void render () const;
  };

} // ed900::menu
 
#endif // __ED900_MENU_X01__

