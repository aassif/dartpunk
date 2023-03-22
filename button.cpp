#include "button.h"

namespace dartpunk
{

  Button::Button (const Duration & d) :
    d {d},
    t0 {}
  {
  }

  // Button down.
  void Button::operator= (const TimePoint & t)
  {
    t0 = t + d;
  }

  // Duration.
  OptDuration Button::operator() (const TimePoint & t) const
  {
    if (t0)
    {
      Duration dt = t - *t0;
      if (dt >= Duration::zero ()) return dt;
    }
    return OptDuration {};
  }

  // Button up.
  bool Button::operator[] (const TimePoint & t)
  {
    OptDuration o = (*this) (t);
    t0.reset ();
    return o.has_value ();
  }

} // dartpunk

