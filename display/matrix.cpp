#include "matrix.h"

using namespace rgb_matrix;

namespace ed900:display
{

  Matrix::Matrix () :
    matrix {nullptr},
    canvas {nullptr}
  {
    RGBMatrix::Options options;
    options.hardware_mapping = "adafruit-hat";
    options.cols = 64;
    options.rows = 64;
    options.chain_length = 2;
    options.pwm_bits = 11;
    options.brightness = 100;
    RuntimeOptions runtime;
    matrix = RGBMatrix::CreateFromOptions (options, runtime);
    canvas = m->CreateFrameCanvas ();
  }

  Matrix::~Matrix ()
  {
    delete matrix;
  }

  void Matrix::operator() (const App:Matrix & m)
  {
    for (uint8_t y = 0; y < HEIGHT; ++y)
      for (uint8_t x = 0; x < WIDTH; ++x)
      {
        const Color & c = m [y][x];
        canvas->SetPixel (x, y, c.r, c.g, c.b);
      }

    canvas = matrix->SwapOnVSync (canvas);
  }
  
}
