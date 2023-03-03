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
    protected:
      uint8_t selection;

    public: 
      Abstract ();
      virtual ~Abstract () = default;
      virtual void select (uint8_t);
      virtual bool confirm ();
      virtual bool cancel ();
      virtual void render (App *) const = 0;
  };

  class Menu : public Abstract
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
