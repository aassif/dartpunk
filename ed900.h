#ifndef __ED900__
#define __ED900__

#include <string>
#include <vector>
#include <functional>
#include <map>
#include <thread>
#include <atomic>
#include <memory>
#include <queue>
#include <mutex>

#include <systemd/sd-bus.h>

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

  struct Device
  {
    std::string path;
    std::string name;
  };

  struct GattChar
  {
    std::string path;
    std::string uuid;
  };

  class ED900
  {
    public:
      typedef std::shared_ptr<Event> EventPtr;

    private:
      // InterfacesAdded.
      typedef std::function<void (const std::string & path, sd_bus_message *)> InterfaceHandler;
      typedef std::map<std::string, InterfaceHandler> InterfaceHandlerMap;
      // PropertiesChanged.
      typedef std::function<void (sd_bus_message *)> PropertyHandler;
      typedef std::map<std::string, PropertyHandler> PropertyHandlerMap;

    private:
      sd_bus * bus;
      std::string adapter_path;
      sd_bus_slot * adapter_slot;
      std::string device_path;
      sd_bus_slot * device_slot;
      std::string gatt_char_path;
      sd_bus_slot * gatt_char_slot;
      std::thread thread;
      std::atomic<bool> stopped;
      std::queue<EventPtr> queue;
      std::mutex queue_mutex;

    private:
      // SD-bus objects.
      void objects (const InterfaceHandlerMap &);
      // BlueZ discovery.
      bool find (const std::string & adapter);
      // BlueZ connection.
      bool connect (const std::string & device);
      // BlueZ subscription.
      bool subscribe (const std::string & characteristic);
      // SD-bus event loop.
      bool loop ();

    private:
      // Device events.
      void onDeviceConnected        (sd_bus_message *);
      void onDeviceServicesResolved (sd_bus_message *);
      // Notifications.
      void onValueChanged           (sd_bus_message *);
      // Events.
      void emitConnectionEvent (bool);
      void emitDartEvent (uint8_t value, uint8_t multiplier);
      void emitButtonEvent (Button);

    public:
      ED900 ();
      ~ED900 ();
      void start (const std::string & adapter);
      void stop ();
      EventPtr poll ();

    private:
      // SD-bus match.
      static std::string Match (const std::string & interface,
                                const std::string & path,
                                const std::string & member,
                                const std::string & type);
      // SD-bus objects.
      static void Objects (const InterfaceHandlerMap &, sd_bus_message *);
      // SD-bus interfaces.
      static void Interfaces (const std::string & path, const InterfaceHandlerMap &, sd_bus_message *);
      static int InterfacesAdded (const InterfaceHandlerMap &, sd_bus_message *);
      // SD-bus properties.
      static void Properties (const PropertyHandlerMap &, sd_bus_message *);
      static int PropertiesChanged (const PropertyHandlerMap &, sd_bus_message *);
      // Debug.
      static std::string Hex (const std::vector<uint8_t> &);
      static void Dump (const std::vector<uint8_t> &);
  };
}

#endif // __ED900__

