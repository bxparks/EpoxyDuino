/*
 * StdioSerialWrite makes ad-hoc calls to various print(), println(), and
 * write() output methods in the `Serial` object to verify that they work as
 * expected. This cannot be written as an AUnit unit test because we want to
 * test the StdioSerial class itself, not the Print parent class.
 * 
 * On Linux or Mac, type:
 *  * $ make
 *  * $ ./StdioSerialWrite.out
 *
 * Should print out the following:
 *
 * ==== Testing print(s)
 * print(s)
 * ==== Testing println(s)
 * println(s)
 * ==== Testing print(F(s))
 * print(F(s))
 * ==== Testing println(F(s))
 * println(F(s))
 * ==== Testing write(c)
 * w
 * ==== Testing write(s)
 * some string
 * ==== Testing write(buf, n)
 * some 
 */

#include <Arduino.h>

void setup() {
#if ! defined(EPOXY_DUINO)
  delay(1000); // some boards reboot twice
#endif

  SERIAL_PORT_MONITOR.begin(115200); // baud ignored on EpoxyDuino
  while (!SERIAL_PORT_MONITOR); // Wait on Leonardo/Micro, no-op on EpoxyDuino

#if defined(EPOXY_DUINO)
  SERIAL_PORT_MONITOR.setLineModeUnix();
#endif

  size_t n;

  SERIAL_PORT_MONITOR.println(F("==== Testing print(s)"));
  n = SERIAL_PORT_MONITOR.print("print(s)");
  SERIAL_PORT_MONITOR.println();
  if (n != 8) {
    SERIAL_PORT_MONITOR.println("print(s) should have returned 8");
  }

  SERIAL_PORT_MONITOR.println(F("==== Testing println(s)"));
  n = SERIAL_PORT_MONITOR.println("println(s)");
  if (n != 11) { // 10 + newline
    SERIAL_PORT_MONITOR.println("print(s) should have returned 11");
  }

  SERIAL_PORT_MONITOR.println(F("==== Testing print(F(s))"));
  n = SERIAL_PORT_MONITOR.print(F("print(F(s))"));
  SERIAL_PORT_MONITOR.println();
  if (n != 11) {
    SERIAL_PORT_MONITOR.println(F("print(F(s)) should have returned 11"));
  }

  SERIAL_PORT_MONITOR.println(F("==== Testing println(F(s))"));
  n = SERIAL_PORT_MONITOR.println(F("println(F(s))"));
  if (n != 14) { // 13 + newline
    SERIAL_PORT_MONITOR.println(F("println(F(s)) should have returned 14"));
  }

  SERIAL_PORT_MONITOR.println(F("==== Testing write(c)"));
  n = SERIAL_PORT_MONITOR.write('w');
  SERIAL_PORT_MONITOR.println();
  if (n != 1) {
    SERIAL_PORT_MONITOR.println(F("write(c) should have returned 1"));
  }

  const char buf[] = "some string"; // strlen(buf) == 11
  SERIAL_PORT_MONITOR.println(F("==== Testing write(s)"));
  n = SERIAL_PORT_MONITOR.write(buf);
  SERIAL_PORT_MONITOR.println();
  if (n != 11) {
    SERIAL_PORT_MONITOR.println(F("write(s) should have returned 11"));
  }

  SERIAL_PORT_MONITOR.println(F("==== Testing write(buf, n)"));
  n = SERIAL_PORT_MONITOR.write((const uint8_t*) buf, 5);
  SERIAL_PORT_MONITOR.println();
  if (n != 5) {
    SERIAL_PORT_MONITOR.println(F("write(buf, n) should have returned 5"));
  }

#if defined(EPOXY_DUINO)
  exit(0);
#endif
}

void loop() {}
