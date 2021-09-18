#line 2 "IPAddressTest"

#include <Arduino.h>
#include <AUnit.h>
#include <IPAddress.h> // IPAddress
#include <AceCommon.h> // PrintStr<N>

using aunit::TestRunner;
using ace_common::PrintStr;

//---------------------------------------------------------------------------

test(IPAddressTest, INADDR_NONE) {
  IPAddress addr(0, 0, 0, 0);
  assertTrue(addr == INADDR_NONE);
}

test(IPAddressTest, printTo) {
  IPAddress addr(127, 0, 0, 1);

  PrintStr<16> ps;
  addr.printTo(ps);
  assertEqual(ps.cstr(), F("127.0.0.1"));
}

#if defined(EPOXY_CORE_ESP8266)

test(IPAddressTest, toString) {
  IPAddress addr(127, 0, 0, 1);

  String s = addr.toString();
  assertEqual(s, F("127.0.0.1"));
}

#endif

//---------------------------------------------------------------------------

void setup() {
#if ! defined(EPOXY_DUINO)
  delay(1000); // wait to prevent garbage on SERIAL_PORT_MONITOR
#endif

  SERIAL_PORT_MONITOR.begin(115200);
  while (!SERIAL_PORT_MONITOR); // needed for Leonardo/Micro
}

void loop() {
  TestRunner::run();
}
