#include "board.h"

using namespace std;

namespace dartpunk::board
{

  void Board::emitConnectionEvent (bool connected)
  {
    lock_guard<mutex> lock {queue_mutex};
    queue.emplace (make_shared<ConnectionEvent> (connected));
  }

  void Board::emitButtonEvent (Button button)
  {
    lock_guard<mutex> lock {queue_mutex};
    queue.emplace (make_shared<ButtonEvent> (button));
  }

  void Board::emitDartEvent (uint8_t value, uint8_t multiplier)
  {
    lock_guard<mutex> lock {queue_mutex};
    queue.emplace (make_shared<DartEvent> (value, multiplier));
  }

  EventPtr Board::poll ()
  {
    lock_guard<mutex> lock {queue_mutex};

    if (queue.empty ())
      return EventPtr {};

    EventPtr e = queue.front ();
    queue.pop ();

    return e;
  }

} // dartpunk::board

