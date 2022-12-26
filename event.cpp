#include "event.h"

using namespace std;

namespace ed900
{

  Event::Event (EventType t) :
    type {t}
  {
  }

  ConnectionEvent::ConnectionEvent (bool c) :
    Event {EventType::CONNECTION},
    connected {c}
  {
  }

  DartEvent::DartEvent (uint8_t v, uint8_t m) :
    Event {EventType::DART},
    value {v},
    multiplier {m}
  {
  }

  string prefix (uint8_t m, bool abbr)
  {
    switch (m)
    {
      case 1: return "";
      case 2: return abbr ? "D" : "Double ";
      case 3: return abbr ? "T" : "Triple ";
      default: return "?";
    }
  }

  string DartEvent::text (bool abbr) const
  {
    return multiplier != 0 ? prefix (multiplier, abbr) + to_string (value) : "-";
  }
    
  ButtonEvent::ButtonEvent (Button b) :
    Event {EventType::BUTTON},
    button {b}
  {
  }

  string ButtonEvent::text () const
  {
    switch (button)
    {
      case Button::NEXT:   return "NEXT";
      case Button::CANCEL: return "CANCEL";
      default:             return "?";
    }
  }

} // ed900

