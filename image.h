#ifndef __ED900_IMAGE__
#define __ED900_IMAGE__

#include <functional>
#include <cmath>
#include <string>

#include <png.h>

namespace ed900
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
  typedef std::function<Color (const Color & dst)> Filler;
  
  namespace blend
  {
    static auto NONE = [] (const Color & src, const Color & dst) {
      return src;
    };

    static auto ColorFill = [] (const Color & color) {
      return [color] (const Color & dst) {return color;};
    };

    static auto BLEND = [] (const Color & src, const Color & dst) {
      auto f = [a = src.a / 255.0] (uint8_t s, uint8_t d) {
        return static_cast<uint8_t> (round (s * a + d * (1.0 - a)));
      };

      return Color {f (src.r, dst.r), f (src.g, dst.g), f (src.b, dst.b), f (1, dst.a)};
    };

    static auto ColorBlend = [] (const Color & color) {
      return [color] (const Color & dst) {return BLEND (color, dst);};
    };

    static auto MODULATE = [] (const Color & src, const Color & dst) {
      auto f = [] (uint8_t s, uint8_t d) {
        return static_cast<uint8_t> (round (s * d / 255.0));
      };

      return Color {f (src.r, dst.r), f (src.g, dst.g), f (src.b, dst.b), dst.a};
    };

    static auto ColorModulate = [] (const Color & color) {
      return [color] (const Color & dst) {return MODULATE (color, dst);};
    };
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

}

#endif // __ED900_IMAGE__

