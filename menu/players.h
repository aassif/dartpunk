#ifndef __DARTPUNK_MENU_PLAYERS__
#define __DARTPUNK_MENU_PLAYERS__

#include "../image.h"
#include "menu.h"

namespace dartpunk::menu
{

  class PlayerCount : public Selection
  {
    private:
      Image icon;

    public:
      PlayerCount ();
      virtual void render (App *) const;
  };

  class PlayerEditor : public Selection
  {
    private:
      uint8_t color;
      std::string name;

    public:
      PlayerEditor ();
      void init (uint8_t id, const std::string & = "");
      virtual void select (uint8_t k);
      operator std::string () const;
      virtual void render (App *) const;

    private:
      static const char KEYBOARD [];
  };

  class Players : public Selection
  {
    private:
      enum class State : uint8_t
      {
        COUNT = 1,
        ADD = 2,
        EDIT = 3,
        SELECT = 4
      };

      struct Player
      {
        std::string name;
        uint8_t state;
      };

    private:
      State                state;
      PlayerCount          count;
      PlayerEditor         editor;
      std::vector<Player>  config;
      std::vector<uint8_t> result;

    private:
      uint8_t find_prev (uint8_t) const;
      uint8_t find_next (uint8_t) const;
      uint8_t find_first () const;
      void write () const;

    public:
      Players ();
      virtual void select (uint8_t k);
      virtual bool confirm ();
      virtual bool cancel ();
      operator std::vector<std::string> () const;
      virtual void render (App *) const;
  };

} // dartpunk::menu
 
#endif // __DARTPUNK_MENU_PLAYERS__

