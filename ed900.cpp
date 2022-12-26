#include <iostream>
#include <iomanip>

#include "ed900.h"

#define BLUEZ "org.bluez"
#define BLUEZ_ADAPTER "org.bluez.Adapter1"
#define BLUEZ_DEVICE "org.bluez.Device1"
#define BLUEZ_GATT_SERVICE "org.bluez.GattService1"
#define BLUEZ_GATT_CHARACTERISTIC "org.bluez.GattCharacteristic1"

#define ED900_COOLDOWN 1000ms

using namespace std;
using namespace chrono;
using namespace literals::chrono_literals;

namespace ed900
{

  ED900::ED900 () :
    bus {nullptr},
    adapter_path {},
    adapter_slot {},
    device_path {},
    device_slot {nullptr},
    gatt_char_path {},
    gatt_char_slot {nullptr},
    thread {},
    stopped {false},
    queue {},
    queue_mutex {}
  {
    sd_bus_open_system (&bus);
  }

  ED900::~ED900 ()
  {
    sd_bus_unref (bus);
  }

  void ED900::Properties (const PropertyHandlerMap & handlers, sd_bus_message * m)
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

  void ED900::Interfaces (const string & path, const InterfaceHandlerMap & handlers, sd_bus_message * m)
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

  void ED900::Objects (const InterfaceHandlerMap & handlers, sd_bus_message * m)
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

  void ED900::objects (const InterfaceHandlerMap & h)
  {
    sd_bus_error e = SD_BUS_ERROR_NULL;
    sd_bus_message * m = nullptr;
    sd_bus_call_method (bus, BLUEZ, "/", "org.freedesktop.DBus.ObjectManager", "GetManagedObjects", &e, &m, NULL);
    Objects (h, m);
    sd_bus_error_free (&e);
    sd_bus_message_unref (m);
  }

  string ED900::Match (const string & interface,
                       const string & path,
                       const string & member,
                       const string & type)
  {
    return "interface='" + interface + "',member='" + member + "',path='" + path + "',type='" + type + "'";
  }

  int ED900::InterfacesAdded (const InterfaceHandlerMap & handlers, sd_bus_message * m)
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
  int ED900::PropertiesChanged (const PropertyHandlerMap & handlers, sd_bus_message * m)
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

  void ED900::start (const string & adapter)
  {
    thread = std::thread {&ED900::find, this, adapter};
  }

  bool ED900::find (const string & a)
  {
    // Set adapter.
    adapter_path = a;

    auto device = [this] (const string & path, sd_bus_message * m) {
      const char * device_name = "";
      auto name = [&device_name] (sd_bus_message * m) {
        sd_bus_message_read_basic (m, 's', &device_name);
      };
      PropertyHandlerMap handlers {{"Name", name}};
      Properties (handlers, m);
cout << path << " : " << device_name << endl;
      if (! strcmp (device_name, "SDB-BT"))
        connect (path);
    };
    InterfaceHandlerMap handlers {{BLUEZ_DEVICE, device}};
    objects (handlers);

    if (device_path.empty ())
    {
      static auto f = [] (sd_bus_message * m, void * p, sd_bus_error *) {
        ED900 * ed900 = static_cast<ED900 *> (p);
        auto device = [ed900] (const string & path, sd_bus_message * m) {
          const char * device_name = "";
          auto name = [&device_name] (sd_bus_message * m) {
            sd_bus_message_read_basic (m, 's', &device_name);
          };
          PropertyHandlerMap handlers {{"Name", name}};
          Properties (handlers, m);
cout << path << " : " << device_name << endl;
          if (! strcmp (device_name, "SDB-BT"))
          {
            sd_bus_error e = SD_BUS_ERROR_NULL;
            sd_bus_call_method (ed900->bus, BLUEZ, ed900->adapter_path.c_str (), BLUEZ_ADAPTER, "StopDiscovery", &e, NULL, NULL);
            sd_bus_error_free (&e);
            ed900->connect (path);
          }
        };
        InterfaceHandlerMap handlers {{BLUEZ_DEVICE, device}};
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

  bool ED900::connect (const string & d)
  {
    cout << "Connecting to " << d << "..." << endl;

    // Set device.
    device_path = d;

    static auto f = [] (sd_bus_message * m, void * p, sd_bus_error *) {
      ED900 * e = static_cast<ED900 *> (p);
      PropertyHandlerMap handlers =
      {
        {"Connected",        [e] (auto m) {e->onDeviceConnected (m);}},
        {"ServicesResolved", [e] (auto m) {e->onDeviceServicesResolved (m);}}
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

  void ED900::onDeviceConnected (sd_bus_message * m)
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

  void ED900::onDeviceServicesResolved (sd_bus_message * m)
  {
    int value;
    sd_bus_message_read_basic (m, 'b', &value);
    cout << "ServicesResolved: " << value << endl;

    if (value)
    {
      auto gatt_char = [this] (const string & path, sd_bus_message * m) {
        const char * gatt_char_uuid = "";
        auto uuid = [&gatt_char_uuid] (sd_bus_message * m) {
          sd_bus_message_read_basic (m, 's', &gatt_char_uuid);
        };
        PropertyHandlerMap h {{"UUID", uuid}};
        Properties (h, m);
cout << path << " : " << gatt_char_uuid << endl;
        static const string UUID {"0000fff1-0000-1000-8000-00805f9b34fb"};
        if (gatt_char_uuid == UUID)
          subscribe (path);
      };
      InterfaceHandlerMap handlers {{BLUEZ_GATT_CHARACTERISTIC, gatt_char}};
      objects (handlers);
    }
  }

#if 0
  void ED900::onDeviceUUIDs (sd_bus_message * m)
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

  bool ED900::subscribe (const string & c)
  {
    // Set characteristic.
    gatt_char_path = c;

    static auto f = [] (sd_bus_message * m, void * p, sd_bus_error *) {
      ED900 * e = static_cast<ED900 *> (p);
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

  void ED900::onValueChanged (sd_bus_message * m)
  {
    const uint8_t * data = nullptr;
    size_t n = 0;
    int r = sd_bus_message_read_array (m, 'y', (const void **) &data, &n);
    vector<uint8_t> d {data, data + n};
    ED900::Dump (d);

    // Previous timestamp.
    static chrono::steady_clock::time_point t0;
    // Event timestamp.
    auto t = steady_clock::now ();

    if (n == 10 && t > t0 + ED900_COOLDOWN)
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

  bool ED900::loop ()
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

  void ED900::stop ()
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

  ED900::EventPtr ED900::poll ()
  {
    lock_guard<mutex> lock {queue_mutex};

    if (queue.empty ())
      return EventPtr {};

    EventPtr e = queue.front ();
    queue.pop ();

    return e;
  }

  string ED900::Hex (const vector<uint8_t> & data)
  {
    ostringstream oss;

    for (auto d : data)
      oss << hex << setw (2) << setfill ('0') << (int) d;

    return oss.str ();
  }

  void ED900::Dump (const vector<uint8_t> & data)
  {
    cout << Hex (data) << endl;
  }

  void ED900::emitConnectionEvent (bool connected)
  {
    lock_guard<mutex> lock {queue_mutex};
    queue.emplace (make_shared<ConnectionEvent> (connected));
  }

  void ED900::emitButtonEvent (Button button)
  {
    lock_guard<mutex> lock {queue_mutex};
    queue.emplace (make_shared<ButtonEvent> (button));
  }

  void ED900::emitDartEvent (uint8_t value, uint8_t multiplier)
  {
    lock_guard<mutex> lock {queue_mutex};
    queue.emplace (make_shared<DartEvent> (value, multiplier));
  }

}

