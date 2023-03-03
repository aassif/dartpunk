#ifndef __DARTPUNK_IMAGE__
#define __DARTPUNK_IMAGE__

#include <functional>
#include <cmath>
#include <string>

#include <png.h>

namespace dartpunk
{

  struct Point
  {
    int x, y;
  };

  struct Rect : public Point
  {
    int w, h;
  };

  struct Color {uint8_t r, g, b, a;};

  typedef std::function<Color (const Color & src, const Color & dst)> Blender;
  
  namespace blend
  {
    static auto NONE = [] (const Color & src, const Color & dst) {
      return src;
    };

    static auto ALPHA = [] (const Color & src, const Color & dst) {
      auto f = [a = src.a / 255.0] (uint8_t s, uint8_t d) {
        return static_cast<uint8_t> (round (s * a + d * (1.0 - a)));
      };

      return Color {f (src.r, dst.r), f (src.g, dst.g), f (src.b, dst.b), f (1, dst.a)};
    };

    static auto MODULATE = [] (const Color & src, const Color & dst) {
      auto f = [] (uint8_t s, uint8_t d) {
        return static_cast<uint8_t> (round (s * d / 255.0));
      };

      return Color {f (src.r, dst.r), f (src.g, dst.g), f (src.b, dst.b), dst.a};
    };

    inline Blender ModAlpha (const Color & c)
    {
      return [c] (const Color & src, const Color & dst) {
        return ALPHA (MODULATE (c, src), dst);
      };
    }
  }

  class Image
  {
    private:
      png_image png;
      png_bytep buffer;

    public:
      Image (const std::string & filename);
      Image (Image &&);
      ~Image ();
      const Color * operator[] (int16_t y) const;
  };

} // dartpunk

#endif // __DARTPUNK_IMAGE__

