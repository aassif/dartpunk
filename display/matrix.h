#ifndef __ED900_DISPLAY_MATRIX__
#define __ED900_DISPLAY_MATRIX__

#include "led-matrix.h"

namespace ed900::display
{

  class Matrix
  {
    private:
      rgb_matrix::RGBMatrix * matrix;
      rgb_matrix::FrameCanvas * canvas;

    public:
      Matrix ();
      ~Matrix ();

      void operator() (const App::Matrix &);
  };

}

#endif // __ED900_DISPLAY_MATRIX__

