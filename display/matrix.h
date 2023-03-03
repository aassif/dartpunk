#ifndef __DARTPUNK_DISPLAY_MATRIX__
#define __DARTPUNK_DISPLAY_MATRIX__

#include "led-matrix.h"

namespace dartpunk::display
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

#endif // __DARTPUNK_DISPLAY_MATRIX__

