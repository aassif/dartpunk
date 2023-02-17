#ifndef __ED900_BOARD__
#define __ED900_BOARD__

#include <queue>
#include <mutex>

#include "event.h"

namespace ed900::board
{

  class Board
  {
    private:
      std::queue<EventPtr> queue;
      std::mutex queue_mutex;

    protected:
      void emitConnectionEvent (bool);
      void emitDartEvent (uint8_t value, uint8_t multiplier);
      void emitButtonEvent (Button);

    public:
      Board () = default;
      virtual ~Board () = default;

      EventPtr poll ();
  };

}

#endif // __ED900_BOARD__

