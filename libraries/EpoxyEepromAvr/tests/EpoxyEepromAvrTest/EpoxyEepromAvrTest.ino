#line 2 "EpoxyEepromAvrTest.ino"

#include <Arduino.h>
#include <AUnit.h>
#include <EpoxyEepromAvr.h>

using aunit::TestRunner;

#define EEPROM EpoxyEepromAvrInstance

//---------------------------------------------------------------------------

test(writeAndReadTest) {
  assertEqual((uint16_t) 1024, EEPROM.length());

  // Write using put()
  uint32_t output = 0x04030201;
  EEPROM.put(0, output);

  // Write at the end of the EEPROM space, using write() and operator[].
  EEPROM.write(1020, 1);
  EEPROM.update(1021, 2);
  EEPROM[1022] = 3;
  EEPROM[1023] = 4;

  // Read using get()
  uint32_t input = 0;
  EEPROM.get(0, input);
  assertEqual(output, input);

  // Read using read() and operator[].
  assertEqual(1, EEPROM.read(1020));
  assertEqual(2, EEPROM.read(1021));
  assertEqual(3, EEPROM[1022]);
  assertEqual(4, EEPROM[1023]);

  // Test iterators and postfix++ on EEPtr.
  int index = 0;
  for (EEPtr p = EEPROM.begin(); p != EEPROM.end(); p++, index++) {
    if (index == 0) assertEqual(1, *p);
    if (index == 1) assertEqual(2, *p);
    if (index == 2) assertEqual(3, *p);
    if (index == 3) assertEqual(4, *p);
  }

  // Test iterators and prefix++ on EEPtr.
  index = 0;
  for (EEPtr p = EEPROM.begin(); p != EEPROM.end(); ++p, ++index) {
    if (index == 1020) assertEqual(1, *p);
    if (index == 1021) assertEqual(2, *p);
    if (index == 1022) assertEqual(3, *p);
    if (index == 1023) assertEqual(4, *p);
  }
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
