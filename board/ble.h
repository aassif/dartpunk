#ifndef __ED900_BOARD_BLE__
#define __ED900_BOARD_BLE__

#include <vector>
#include <functional>
#include <map>
#include <thread>
#include <atomic>

#include <systemd/sd-bus.h>

#include "board.h"

namespace ed900::board
{

  class BLE : public Board
  {
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
      std::string device_name;
      std::string device_path;
      sd_bus_slot * device_slot;
      std::string gatt_char_uuid;
      std::string gatt_char_path;
      sd_bus_slot * gatt_char_slot;
      std::thread thread;
      std::atomic<bool> stopped;

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

    protected:
      virtual void onValueChanged (const std::vector<uint8_t> &) = 0;

    public:
      BLE (const std::string & adapter,
           const std::string & name,
           const std::string & uuid);
      virtual ~BLE ();

    private:
      void start (const std::string & adapter);
      void stop ();

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

    protected:
      // Debug.
      static std::string Hex (const std::vector<uint8_t> &);
      static void Dump (const std::vector<uint8_t> &);
  };

} // ed900::board

#endif // __ED900_BOARD_BLE__

