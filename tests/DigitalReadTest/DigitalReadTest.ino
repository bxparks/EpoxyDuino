#line 2 "DigitalReadTest"

#include <Arduino.h>
#include <AUnit.h>

using aunit::TestRunner;

//---------------------------------------------------------------------------

test(DigitalReadTest, digitalReadValue_valid) {
  // Verify that pins return 0 initially.
  assertEqual(digitalRead(0), 0);
  assertEqual(digitalRead(31), 0);
  assertEqual(digitalRead(32), 0);

  // Set the values of those pins to 1. Pin 32 should be a no-op.
  digitalReadValue(0, 1);
  digitalReadValue(31, 1);
  digitalReadValue(32, 1);

  // Check that those pins return 1. Pin 32 continues to return 0.
  assertEqual(digitalRead(0), 1);
  assertEqual(digitalRead(31), 1);
  assertEqual(digitalRead(32), 0);

  // Set the values of those pins to 0. Pin 32 should be a no-op.
  digitalReadValue(0, 0);
  digitalReadValue(31, 0);
  digitalReadValue(32, 0);

  // Check that those pins return 0.
  assertEqual(digitalRead(0), 0);
  assertEqual(digitalRead(31), 0);
  assertEqual(digitalRead(32), 0);
}

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
