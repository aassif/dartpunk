#include <chrono>
#include <optional>

namespace dartpunk
{

  typedef std::chrono::steady_clock::duration Duration;
  typedef std::optional<Duration> OptDuration;

  typedef std::chrono::steady_clock::time_point TimePoint;
  typedef std::optional<TimePoint> OptTimePoint;

  class Button
  {
    private:
      const Duration d;
      OptTimePoint t0;

    public:
      Button (const Duration & d);
      // Button down.
      void operator= (const TimePoint &);
      // Duration.
      OptDuration operator() (const TimePoint &) const;
      // Button up.
      bool operator[] (const TimePoint &);
  };

} // dartpunk

