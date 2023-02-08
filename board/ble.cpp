#include <iostream>
#include <iomanip>

#include "ble.h"

#define BLUEZ "org.bluez"
#define BLUEZ_ADAPTER "org.bluez.Adapter1"
#define BLUEZ_DEVICE "org.bluez.Device1"
#define BLUEZ_GATT_SERVICE "org.bluez.GattService1"
#define BLUEZ_GATT_CHARACTERISTIC "org.bluez.GattCharacteristic1"

using namespace std;

namespace ed900::board
{

  BLE::BLE (const string & adapter,
            const string & name,
            const string & uuid) :
    bus {nullptr},
    adapter_path {},
    adapter_slot {},
    device_name {name},
    device_path {},
    device_slot {nullptr},
    gatt_char_uuid {uuid},
    gatt_char_path {},
    gatt_char_slot {nullptr},
    thread {},
    stopped {false}
  {
    sd_bus_open_system (&bus);
    start (adapter);
  }

  BLE::~BLE ()
  {
    stop ();
    sd_bus_unref (bus);
  }

  void BLE::Properties (const PropertyHandlerMap & handlers, sd_bus_message * m)
  {
    auto c0 = [m] () {return sd_bus_message_enter_container (m, 'a', "{sv}");};
    auto c1 = [m] () {return sd_bus_message_enter_container (m, 'e', "sv");};

    c0 ();
    for (int r1 = c1 (); r1 > 0; r1 = c1 ())
    {
      char * property = nullptr;
      sd_bus_message_read_basic (m, 's', &property);

      auto f = handlers.find (property);
      if (f != handlers.end ())
      {
        const char * variant = nullptr;
        sd_bus_message_peek_type (m, NULL, &variant);
        sd_bus_message_enter_container (m, 'v', variant);

        (f->second) (m);

        sd_bus_message_exit_container (m);
      }
      else
        sd_bus_message_skip (m, "v");

      sd_bus_message_exit_container (m); // c1
    }
    sd_bus_message_exit_container (m); // c0
  }

  void BLE::Interfaces (const string & path, const InterfaceHandlerMap & handlers, sd_bus_message * m)
  {
    auto c0 = [m] () {return sd_bus_message_enter_container (m, 'a', "{sa{sv}}");};
    auto c1 = [m] () {return sd_bus_message_enter_container (m, 'e', "sa{sv}");};

    c0 ();
    for (int r1 = c1 (); r1 > 0; r1 = c1 ())
    {
      char * interface = nullptr;
      sd_bus_message_read_basic (m, 's', &interface);

      auto f = handlers.find (interface);
      if (f != handlers.end ())
        (f->second) (path, m);
      else
        sd_bus_message_skip (m, "a{sv}");

      sd_bus_message_exit_container (m); // c1
    }
    sd_bus_message_exit_container (m); // c0
  }

  void BLE::Objects (const InterfaceHandlerMap & handlers, sd_bus_message * m)
  {
    auto c0 = [m] () {return sd_bus_message_enter_container (m, 'a', "{oa{sa{sv}}}");};
    auto c1 = [m] () {return sd_bus_message_enter_container (m, 'e', "oa{sa{sv}}");};

    c0 ();
    for (int r1 = c1 (); r1 > 0; r1 = c1 ())
    {
      char * path = nullptr;
      sd_bus_message_read_basic (m, 'o', &path);

      Interfaces (path, handlers, m);

      sd_bus_message_exit_container (m); // c1
    }
    sd_bus_message_exit_container (m); // c0
  }

  void BLE::objects (const InterfaceHandlerMap & h)
  {
    sd_bus_error e = SD_BUS_ERROR_NULL;
    sd_bus_message * m = nullptr;
    sd_bus_call_method (bus, BLUEZ, "/", "org.freedesktop.DBus.ObjectManager", "GetManagedObjects", &e, &m, NULL);
    Objects (h, m);
    sd_bus_error_free (&e);
    sd_bus_message_unref (m);
  }

  string BLE::Match (const string & interface,
                       const string & path,
                       const string & member,
                       const string & type)
  {
    return "interface='" + interface + "',member='" + member + "',path='" + path + "',type='" + type + "'";
  }

  int BLE::InterfacesAdded (const InterfaceHandlerMap & handlers, sd_bus_message * m)
  {
    //cout << "InterfacesAdded" << endl;

    // Path.
    char * path = nullptr;
    sd_bus_message_read_basic (m, 'o', &path);
#if 0
    cout << "  " << path << endl;
#endif

    Interfaces (path, handlers, m);
    return 0;
  }

  // "sa{sv}as"
  int BLE::PropertiesChanged (const PropertyHandlerMap & handlers, sd_bus_message * m)
  {
    //cout << "PropertiesChanged" << endl;

    // Interface.
    char * interface = nullptr;
    sd_bus_message_read_basic (m, 's', &interface);
#if 0
    cout << "  " << interface << endl;
#endif

    // Changed properties.
    Properties (handlers, m);

    // Invalidated properties.
    char ** str = nullptr;
    sd_bus_message_read_strv (m, &str);
    if (str)
    {
      for (uint8_t k = 0; str[k]; ++k)
        cout << "  - " << str[k] << endl;

      free (str);
    }

    return 0;
  }

  void BLE::start (const string & adapter)
  {
    thread = std::thread {&BLE::find, this, adapter};
  }

  bool BLE::find (const string & a)
  {
    // Set adapter.
    adapter_path = a;

    static auto device = [] (BLE * ble, bool stop) {
      return [ble, stop] (const string & path, sd_bus_message * m) {
        const char * n = "";
        auto name = [&n] (sd_bus_message * m) {
          sd_bus_message_read_basic (m, 's', &n);
        };
        PropertyHandlerMap handlers {{"Name", name}};
        Properties (handlers, m);
cout << path << " : " << n << endl;
        if (n == ble->device_name)
        {
          if (stop)
          {
            sd_bus_error e = SD_BUS_ERROR_NULL;
            sd_bus_call_method (ble->bus, BLUEZ, ble->adapter_path.c_str (), BLUEZ_ADAPTER, "StopDiscovery", &e, NULL, NULL);
            sd_bus_error_free (&e);
          }
          ble->connect (path);
        }
      };
    };

    InterfaceHandlerMap handlers {{BLUEZ_DEVICE, device (this, false)}};
    objects (handlers);

    if (device_path.empty ())
    {
      static auto f = [] (sd_bus_message * m, void * p, sd_bus_error *) {
        BLE * ble = static_cast<BLE *> (p);
        InterfaceHandlerMap handlers {{BLUEZ_DEVICE, device (ble, true)}};
        return InterfacesAdded (handlers, m);
      };

      string match = Match ("org.freedesktop.DBus.ObjectManager", "/", "InterfacesAdded", "signal");
      sd_bus_add_match (bus, &adapter_slot, match.c_str (), f, (void *) this);

      sd_bus_error e = SD_BUS_ERROR_NULL;
      sd_bus_call_method (bus, BLUEZ, adapter_path.c_str (), BLUEZ_ADAPTER, "StartDiscovery", &e, NULL, NULL);
      sd_bus_error_free (&e);
    }

    return loop ();
  }

  bool BLE::connect (const string & d)
  {
    cout << "Connecting to " << d << "..." << endl;

    // Set device.
    device_path = d;

    static auto f = [] (sd_bus_message * m, void * p, sd_bus_error *) {
      BLE * ble = static_cast<BLE *> (p);
      PropertyHandlerMap handlers =
      {
        {"Connected",        [ble] (auto m) {ble->onDeviceConnected (m);}},
        {"ServicesResolved", [ble] (auto m) {ble->onDeviceServicesResolved (m);}}
      };
      return PropertiesChanged (handlers, m);
    };

    string match = Match ("org.freedesktop.DBus.Properties", device_path, "PropertiesChanged", "signal");
    sd_bus_add_match (bus, &device_slot, match.c_str (), f, (void *) this);

    sd_bus_error e = SD_BUS_ERROR_NULL;
    int r = sd_bus_call_method (bus, BLUEZ, device_path.c_str (), BLUEZ_DEVICE, "Connect", &e, NULL, NULL);
    sd_bus_error_free (&e);

    return r >= 0;
  }

  void BLE::onDeviceConnected (sd_bus_message * m)
  {
    int value;
    sd_bus_message_read_basic (m, 'b', &value);
    cout << "Connected: " << value << endl;

    emitConnectionEvent (value);

    if (! value)
    {
      gatt_char_slot = sd_bus_slot_unref (gatt_char_slot);
      device_slot = sd_bus_slot_unref (device_slot);
      connect (device_path);
    }
  }

  void BLE::onDeviceServicesResolved (sd_bus_message * m)
  {
    int value;
    sd_bus_message_read_basic (m, 'b', &value);
    cout << "ServicesResolved: " << value << endl;

    if (value)
    {
      auto gatt_char = [this] (const string & path, sd_bus_message * m) {
        const char * u = "";
        auto uuid = [&u] (sd_bus_message * m) {
          sd_bus_message_read_basic (m, 's', &u);
        };
        PropertyHandlerMap h {{"UUID", uuid}};
        Properties (h, m);
cout << path << " : " << u << endl;
        if (u == gatt_char_uuid)
          subscribe (path);
      };
      InterfaceHandlerMap handlers {{BLUEZ_GATT_CHARACTERISTIC, gatt_char}};
      objects (handlers);
    }
  }

#if 0
  void BLE::onDeviceUUIDs (sd_bus_message * m)
  {
    char ** data = nullptr;
    int r = sd_bus_message_read_strv (m, &data);

    if (data)
    {
      cout << "  + UUIDs:" << r << endl;

      for (uint8_t k = 0; data[k]; ++k)
        cout << "      " << data[k] << endl;

      free (data);
    }
  }
#endif

  bool BLE::subscribe (const string & c)
  {
    // Set characteristic.
    gatt_char_path = c;

    static auto f = [] (sd_bus_message * m, void * p, sd_bus_error *) {
      BLE * e = static_cast<BLE *> (p);
      PropertyHandlerMap handlers =
      {
        {"Value", [e] (auto m) {e->onValueChanged (m);}}
      };
      return PropertiesChanged (handlers, m);
    };

    string match = Match ("org.freedesktop.DBus.Properties", gatt_char_path, "PropertiesChanged", "signal");
    sd_bus_add_match (bus, &gatt_char_slot, match.c_str (), f, (void *) this);

    sd_bus_error e = SD_BUS_ERROR_NULL;
    int r = sd_bus_call_method (bus, BLUEZ, gatt_char_path.c_str (), BLUEZ_GATT_CHARACTERISTIC, "StartNotify", &e, NULL, NULL);
    sd_bus_error_free (&e);

    return r >= 0;
  }

  void BLE::onValueChanged (sd_bus_message * m)
  {
    const uint8_t * data = nullptr;
    size_t n = 0;
    int r = sd_bus_message_read_array (m, 'y', (const void **) &data, &n);
    onValueChanged ({data, data + n});
  }

  bool BLE::loop ()
  {
    sd_event * event = nullptr;
    sd_event_default (&event);
    sd_bus_attach_event (bus, event, SD_EVENT_PRIORITY_NORMAL);

    while (! stopped)
    {
      if (sd_event_run (event, 100) < 0)
        return false;
    }

    sd_event_unref (event);
    return true;
  }

  void BLE::stop ()
  {
    stopped = true;

    thread.join ();

    if (! device_path.empty ())
    {
      cout << "device: " << device_path << endl;

      if (! gatt_char_path.empty ())
      {
        gatt_char_slot = sd_bus_slot_unref (gatt_char_slot);

        sd_bus_error e = SD_BUS_ERROR_NULL;
        sd_bus_call_method (bus, BLUEZ, gatt_char_path.c_str (), BLUEZ_DEVICE, "StopNotify", &e, NULL, NULL);
        sd_bus_error_free (&e);
      }

      device_slot = sd_bus_slot_unref (device_slot);

      sd_bus_error e = SD_BUS_ERROR_NULL;
      sd_bus_call_method (bus, BLUEZ, device_path.c_str (), BLUEZ_DEVICE, "Disconnect", &e, NULL, NULL);
      sd_bus_error_free (&e);
    }
  }

  string BLE::Hex (const vector<uint8_t> & data)
  {
    ostringstream oss;

    for (auto d : data)
      oss << hex << setw (2) << setfill ('0') << (int) d;

    return oss.str ();
  }

  void BLE::Dump (const vector<uint8_t> & data)
  {
    cout << Hex (data) << endl;
  }

}

