#ifndef __ED900_FONT__
#define __ED900_FONT__

#include <string>
#include <map>
#include <functional>

#include "image.h"

namespace ed900
{

  class App;

  class Font
  {
    protected:
      Image image;
      uint8_t width;
      uint8_t height;
      std::map<char, Rect> index;

    public:
      Font (const std::string & path, uint8_t w, uint8_t h);
      void draw (App *, char, int x, int y);
      void draw (App *, const std::string &, int x, int y);
  };

/*
  class FontPico8 : public Font
  {
    public:
      FontPico8 ();
  };
*/

  class FontTomThumb : public Font
  {
    public:
      FontTomThumb ();
  };

/*
  class FontAmstrad : public Font
  {
    public:
      FontAmstrad ();
  };
*/

  class FontTopaz : public Font
  {
    public:
      FontTopaz ();
  };

  class FontRoboto : public Font
  {
    public:
      FontRoboto ();
  };

/*
  class FontRazor1911 : public Font
  {
    public:
      FontRazor1911 ();
  };

  class FontMagicPockets : public Font
  {
    public:
      FontMagicPockets ();
  };
*/

  class FontNormal : public Font
  {
    public:
      FontNormal ();
  };

  class FontScore : public Font
  {
    public:
      FontScore ();
  };

/*
  class FontLarge : public Font
  {
    public:
      FontLarge ();
  };
*/

  class FontCricket : public Font
  {
    public:
      FontCricket ();
  };

} // ed900

#endif // __ED900_FONT__

