#include <iostream>
#include <cstring>

#include <fcntl.h>
#include <sys/time.h>

#include "device.h"

using namespace std;
using namespace chrono;

namespace dartpunk::input
{

////////////////////////////////////////////////////////////////////////////////

  TimePoint now ()
  {
    timespec ts;
    clock_gettime (CLOCK_REALTIME, &ts);
    return TimePoint {seconds {ts.tv_sec} + nanoseconds {ts.tv_nsec}};
  }

////////////////////////////////////////////////////////////////////////////////

  TimePoint Event::time_point () const
  {
    return TimePoint {seconds {input_event_sec} + microseconds {input_event_usec}};
  }

////////////////////////////////////////////////////////////////////////////////

  Device::Device (const std::string & path) :
    device {nullptr}
  {
    int fd = open (path.c_str (), O_RDONLY|O_NONBLOCK);
    int rc = libevdev_new_from_fd (fd, &device);
    if (rc < 0)
      throw std::runtime_error (strerror (-rc));

    cout << libevdev_get_name (device) << endl;
  }

  Device::~Device ()
  {
    libevdev_free (device);
  }

  EventPtr Device::poll ()
  {
    auto e = make_shared<Event> ();

    int r = libevdev_next_event (device, LIBEVDEV_READ_FLAG_NORMAL, e.get ());
    while (r == LIBEVDEV_READ_STATUS_SYNC)
      r = libevdev_next_event (device, LIBEVDEV_READ_FLAG_SYNC, e.get ());

    return r == LIBEVDEV_READ_STATUS_SUCCESS ? e : nullptr;
  }

} // dartpunk::input

