#ifndef __ED900_MENU__
#define __ED900_MENU__

#include <vector>
#include <string>

namespace ed900
{
  class App;
}

namespace ed900::menu
{

  class Abstract
  {
    protected:
      App * app;
      uint8_t selection;

    public: 
      Abstract (App *);
      virtual ~Abstract () = default;
      virtual void select (uint8_t);
      virtual bool confirm ();
      virtual bool cancel ();
      virtual void render () const = 0;
  };

  class Menu : public Abstract
  {
    private:
      std::vector<std::string> items;

    public:
      Menu (App *, const std::vector<std::string> &);
      ~Menu ();
      uint8_t index ();
      virtual void render () const;
  };

} // ed900::menu

#endif // __ED900_MENU__
