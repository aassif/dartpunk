#ifndef __ED900_MENU_PLAYERS__
#define __ED900_MENU_PLAYERS__

#include "../image.h"
#include "menu.h"

namespace ed900::menu
{

  class Players : public Abstract
  {
    private:
      Image icon;
      std::vector<std::string> config;
      uint8_t count;
      std::vector<uint8_t> states;
      std::vector<uint8_t> result;

    private:
      uint8_t find_prev (uint8_t);
      uint8_t find_next (uint8_t);
      uint8_t find_first ();
      void render_count (App *) const;
      void render_names (App *) const;

    public:
      Players ();
      virtual void select (uint8_t k);
      virtual bool confirm ();
      virtual bool cancel ();
      operator std::vector<std::string> () const;
      virtual void render (App *) const;
  };

} // ed900::menu
 
#endif // __ED900_MENU_PLAYERS__

