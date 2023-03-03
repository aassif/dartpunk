#include "ed900.h"

#define ED900_GATT_CHAR_UUID "0000fff1-0000-1000-8000-00805f9b34fb"
#define ED900_DEVICE_NAME    "SDB-BT"
#define ED900_COOLDOWN       1000ms

using namespace std;
using namespace chrono;
using namespace literals::chrono_literals;

namespace dartpunk::board
{

  ED900::ED900 (const string & adapter) :
    BLE {adapter, ED900_DEVICE_NAME, ED900_GATT_CHAR_UUID}
  {
  }

  void ED900::onValueChanged (const vector<uint8_t> & d)
  {
    BLE::Dump (d);

    // Previous timestamp.
    static chrono::steady_clock::time_point t0;
    // Event timestamp.
    auto t = steady_clock::now ();

    if (d.size () == 10 && t > t0 + ED900_COOLDOWN)
    {
      uint32_t id = d[0] | (d[1] << 8) | (d[2] << 16) | (d[3] << 24);
      uint16_t reserved1 = d[4] | (d[5] << 8);
      uint8_t multiplier = d[6];

      if (multiplier != 0)
      {
        uint8_t units = d[7];
        uint8_t tens = d[8];
        uint8_t reserved2 = d[9];

        switch (tens)
        {
          case 0x0E:
            emitDartEvent (25, 1);
            break;

          case 0x0F:
            emitDartEvent (25, 2);
            break;

          default:
          {
            int value = 10 * tens + units;
            int m = 1;
            switch (multiplier)
            {
              case 0x0B : m = 3; break;
              case 0x0D : m = 2; break;
            }
            emitDartEvent (value, m);
            break;
          }
        }
      }
      else
      {
        uint8_t reserved2 = d[7];
        uint16_t button = d[8] | (d[9] << 8);

        // Previous button.
        static Button b0;

        switch (button)
        {
          // Repeat.
          case 0xCCCC:
            emitButtonEvent (b0);
            break;

          // Cancel.
          case 0xDDDD:
            emitButtonEvent (Button::CANCEL);
            b0 = Button::CANCEL;
            break;

          // Miss.
          case 0xEEEE:
            emitDartEvent (0, 0);
            break;

          // Next.
          case 0xFFFF:
            emitButtonEvent (Button::NEXT);
            b0 = Button::NEXT;
            break;
        }
      }
    }

    // Update timestamp.
    t0 = t;
  }

} // dartpunk::board

