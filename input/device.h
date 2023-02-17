#ifndef __ED900_INPUT_DEVICE__
#define __ED900_INPUT_DEVICE__

#include <string>
#include <memory> // shared_ptr

#include <libevdev/libevdev.h>

namespace ed900::input
{

  typedef struct input_event Event;
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

} // ed900::input

#endif // __ED900_INPUT_DEVICE__

