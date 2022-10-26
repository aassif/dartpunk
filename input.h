#ifndef __ED900_INPUT__
#define __ED900_INPUT__

#include <memory>
#include <string>

#include <libevdev/libevdev.h>

namespace ed900
{

  class Input
  {
    public:
      typedef struct input_event Event;
      typedef std::shared_ptr<Event> EventPtr;

    private:
      struct libevdev * device;

    public:
      Input (const std::string & path);
      ~Input ();

      EventPtr poll ();
  };

} // ed900

#endif // __ED900_INPUT__

