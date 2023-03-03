#ifndef __DARTPUNK_INPUT_DEVICE__
#define __DARTPUNK_INPUT_DEVICE__

#include <string>
#include <memory> // shared_ptr

#include <libevdev/libevdev.h>

namespace dartpunk::input
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

} // dartpunk::input

#endif // __DARTPUNK_INPUT_DEVICE__

