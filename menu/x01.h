#ifndef __DARTPUNK_MENU_X01__
#define __DARTPUNK_MENU_X01__

#include "menu.h"

namespace dartpunk::menu
{

  class X01 : public Abstract
  {
    private:
      bool double_in;
      bool double_out;

    public:
      X01 ();
      ~X01 () = default;
      virtual void select (uint8_t k);
      virtual bool confirm ();
      virtual bool cancel ();
      operator std::pair<bool, bool> () const;
      virtual void render (App *) const;
  };

} // dartpunk::menu
 
#endif // __DARTPUNK_MENU_X01__

