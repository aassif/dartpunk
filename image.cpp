#include <cstring> // memset
#include <utility> // exchange

#include "image.h"

using namespace std;

namespace dartpunk
{

  Image::Image (const string & f) :
    png {},
    buffer {nullptr}
  {
    memset (&png, 0, sizeof (png));
    png.version = PNG_IMAGE_VERSION;
    if (png_image_begin_read_from_file (&png, f.c_str ()) != 0)
    {
      png.format = PNG_FORMAT_RGBA;
      buffer = new png_byte [PNG_IMAGE_SIZE (png)];
      png_image_finish_read (&png, NULL, buffer, 0, NULL);
    }
  }

  Image::Image (Image && image) :
    png {std::exchange (image.png, png_image {})},
    buffer {std::exchange (image.buffer, nullptr)}
  {
  }

  Image::~Image ()
  {
    png_image_free (&png);
    delete[] buffer;
  }

  const Color * Image::operator[] (int16_t y) const
  {
    return reinterpret_cast<const Color *> (buffer + y * PNG_IMAGE_ROW_STRIDE (png));
  }

} // ed900

