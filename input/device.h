#ifndef __DARTPUNK_INPUT_DEVICE__
#define __DARTPUNK_INPUT_DEVICE__

#include <chrono>
#include <memory> // shared_ptr

#include <libevdev/libevdev.h>

namespace dartpunk::input
{

  typedef std::chrono::steady_clock::time_point TimePoint;

  TimePoint now ();

  struct Event : public input_event
  {
    TimePoint time_point () const;
  };

  typedef std::shared_ptr<Event> EventPtr;

  class Device
  {
    private:
      struct libevdev * device;

    public:
      Device (const std::string & path);
      ~Device ();

      EventPtr poll ();
  };

} // dartpunk::input

#endif // __DARTPUNK_INPUT_DEVICE__

