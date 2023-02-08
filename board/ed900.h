#ifndef __ED900_BOARD_ED900__
#define __ED900_BOARD_ED900__

#include "ble.h"

namespace ed900::board
{

  class ED900 : public BLE
  {
    public:
      ED900 (const std::string & adapter);

    protected:
      virtual void onValueChanged (const std::vector<uint8_t> &);
  };

}

#endif // __ED900_BOARD_ED900__

