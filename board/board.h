#ifndef __DARTPUNK_BOARD__
#define __DARTPUNK_BOARD__

#include <queue>
#include <mutex>

#include "event.h"

namespace dartpunk::board
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

} // dartpunk::board

#endif // __DARTPUNK_BOARD__

