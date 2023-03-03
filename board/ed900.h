#ifndef __DARTPUNK_BOARD_ED900__
#define __DARTPUNK_BOARD_ED900__

#include "ble.h"

namespace dartpunk::board
{

  class ED900 : public BLE
  {
    public:
      ED900 (const std::string & adapter);

    protected:
      virtual void onValueChanged (const std::vector<uint8_t> &);
  };

}

#endif // __DARTPUNK_BOARD_ED900__

