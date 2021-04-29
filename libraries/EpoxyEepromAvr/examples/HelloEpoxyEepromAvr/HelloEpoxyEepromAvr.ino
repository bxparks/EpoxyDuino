/*
 * Test the EEPROM implementation provided by EpoxyEepromAvr. This program
 * should compile and run under EpoxyDuino and produce the following output:
 *
 *    write1: 0 1 2 3 252 253 254 255
 *    write2: 3 2 1 0 255 254 253 252
 */

#include <Arduino.h>
#include <EpoxyEepromAvr.h>

#ifndef SERIAL_PORT_MONITOR
#define SERIAL_PORT_MONITOR Serial
#endif

#define EEPROM EpoxyEepromAvrInstance

void writeToEeprom1() {
  // Write at address=0
  uint32_t value = 0x03020100;
  EEPROM.put(0, value);

  // Write at the end of the EEPROM.
  EEPROM.write(1020, 252);
  EEPROM.write(1021, 253);
  EEPROM.write(1022, 254);
  EEPROM.write(1023, 255);
}

void writeToEeprom2() {
  // Write at address=0
  uint32_t value = 0x00010203;
  EEPROM.put(0, value);

  // Write at the end of the EEPROM.
  EEPROM.write(1020, 255);
  EEPROM.write(1021, 254);
  EEPROM.write(1022, 253);
  EEPROM.write(1023, 252);
}

void readFromEeprom(const char* label) {
  SERIAL_PORT_MONITOR.print(label);
  SERIAL_PORT_MONITOR.print(": ");

  SERIAL_PORT_MONITOR.print(EEPROM.read(0));
  SERIAL_PORT_MONITOR.print(' ');
  SERIAL_PORT_MONITOR.print(EEPROM.read(1));
  SERIAL_PORT_MONITOR.print(' ');
  SERIAL_PORT_MONITOR.print(EEPROM.read(2));
  SERIAL_PORT_MONITOR.print(' ');
  SERIAL_PORT_MONITOR.print(EEPROM.read(3));
  SERIAL_PORT_MONITOR.print(' ');

  SERIAL_PORT_MONITOR.print(EEPROM.read(1020));
  SERIAL_PORT_MONITOR.print(' ');
  SERIAL_PORT_MONITOR.print(EEPROM.read(1021));
  SERIAL_PORT_MONITOR.print(' ');
  SERIAL_PORT_MONITOR.print(EEPROM.read(1022));
  SERIAL_PORT_MONITOR.print(' ');
  SERIAL_PORT_MONITOR.print(EEPROM.read(1023));

  SERIAL_PORT_MONITOR.println();
}

void setup() {
#if ! defined(EPOXY_DUINO)
  delay(1000); // some boards reboot twice
#endif
  SERIAL_PORT_MONITOR.begin(115200);
  while (!SERIAL_PORT_MONITOR); // For Leonardo/Micro

  writeToEeprom1();
  readFromEeprom("write1");
  writeToEeprom2();
  readFromEeprom("write2");

#if defined(EPOXY_DUINO)
  exit(0);
#endif
}

void loop() {
}
