#ifndef __DARTPUNK_MENU__
#define __DARTPUNK_MENU__

#include <vector>
#include <string>

namespace dartpunk
{
  class App;
}

namespace dartpunk::menu
{

  class Abstract
  {
    public:
      Abstract () = default;
      virtual ~Abstract () = default;
      virtual void select (uint8_t) = 0;
      virtual bool confirm ();
      virtual bool cancel ();
      virtual void render (App *) const = 0;
  };

  class Selection : public Abstract
  {
    protected:
      uint8_t selection;

    public:
      Selection ();
      virtual ~Selection () = default;
      virtual void select (uint8_t);
      virtual bool confirm ();
      operator uint8_t () const;
      virtual void render (App *) const = 0;
  };

  class Menu : public Selection
  {
    private:
      std::vector<std::string> items;

    public:
      Menu (const std::vector<std::string> &);
      virtual ~Menu () = default;
      uint8_t index ();
      virtual void render (App *) const;
  };

} // dartpunk::menu

#endif // __DARTPUNK_MENU__
