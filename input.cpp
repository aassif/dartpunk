#include <iostream>
#include <cstring>

#include <fcntl.h>

#include "input.h"

using namespace std;

namespace ed900
{

  Input::Input (const std::string & path) :
    device {nullptr}
  {
    int fd = open (path.c_str (), O_RDONLY|O_NONBLOCK);

    int rc = libevdev_new_from_fd (fd, &device);
    if (rc < 0)
      throw std::runtime_error (strerror (-rc));

    cout << libevdev_get_name (device) << endl;
  }

  Input::~Input ()
  {
    libevdev_free (device);
  }

  Input::EventPtr Input::poll ()
  {
    auto e = make_shared<Event> ();
    int r = libevdev_next_event (device, LIBEVDEV_READ_FLAG_NORMAL, e.get ());
    while (r == LIBEVDEV_READ_STATUS_SYNC)
      r = libevdev_next_event (device, LIBEVDEV_READ_FLAG_SYNC, e.get ());
    return r == LIBEVDEV_READ_STATUS_SUCCESS ? e : nullptr;
  }

} // ed900

