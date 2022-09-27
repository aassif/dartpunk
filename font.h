#ifndef __ED900_FONT__
#define __ED900_FONT__

#include <string>
#include <map>

#include <SDL2/SDL.h>

namespace ed900
{
  class Font
  {
    protected:
      uint8_t width;
      uint8_t height;
      SDL_Renderer * renderer;
      SDL_Texture * texture;
      std::map<char, SDL_Rect> index;

    public:
      Font (SDL_Renderer *, const std::string & path, uint8_t w, uint8_t h);
      Font (Font &&);
      virtual ~Font ();
      void draw (char, int x, int y);
      void draw (const std::string &, int x, int y);
  };

/*
  class FontPico8 : public Font
  {
    public:
      FontPico8 (SDL_Renderer *);
  };
*/

  class FontTomThumb : public Font
  {
    public:
      FontTomThumb (SDL_Renderer *);
  };

/*
  class FontAmstrad : public Font
  {
    public:
      FontAmstrad (SDL_Renderer *);
  };
*/

  class FontTopaz : public Font
  {
    public:
      FontTopaz (SDL_Renderer *);
  };

  class FontRoboto : public Font
  {
    public:
      FontRoboto (SDL_Renderer *);
  };

/*
  class FontRazor1911 : public Font
  {
    public:
      FontRazor1911 (SDL_Renderer *);
  };

  class FontMagicPockets : public Font
  {
    public:
      FontMagicPockets (SDL_Renderer *);
  };
*/

  class FontNormal : public Font
  {
    public:
      FontNormal (SDL_Renderer *);
  };

  class FontScore : public Font
  {
    public:
      FontScore (SDL_Renderer *);
  };

/*
  class FontLarge : public Font
  {
    public:
      FontLarge (SDL_Renderer *);
  };
*/

  class FontCricket : public Font
  {
    public:
      FontCricket (SDL_Renderer *);
  };
}

#endif // __ED900_FONT__

