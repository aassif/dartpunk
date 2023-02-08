#ifndef __ED900_EVENT__
#define __ED900_EVENT__

#include <string>
#include <memory> // shared_ptr

namespace ed900
{

  enum class EventType : uint8_t
  {
    CONNECTION = 1,
    DART = 2,
    BUTTON = 3
  };

  class Event
  {
    public:
      EventType type;

    public:
      Event (EventType);
      virtual ~Event () = default;
  };

  class ConnectionEvent : public Event
  {
    public:
      bool connected;

    public:
      ConnectionEvent (bool);
  };

  class DartEvent : public Event
  {
    public:
      uint8_t value;
      uint8_t multiplier;

    public:
      DartEvent (uint8_t value, uint8_t multiplier = 1);
      std::string text (bool abbr = false) const;
  };

  enum class Button : uint8_t
  {
    NEXT = 1,
    CANCEL = 2
  };
    
  class ButtonEvent : public Event
  {
    public:
      Button button;

    public:
      ButtonEvent (Button);
      std::string text () const;
  };

  typedef std::shared_ptr<Event> EventPtr;

} // ed900

#endif // __ED900_EVENT__

