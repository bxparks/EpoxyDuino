/*
 * Test the EEPROM implementation provided by EpoxyPromAvr. This program should
 * compile and run under:
 *
 *    * EpoxyDuino
 *    * Arduino Nano
 *    * SparkRun Pro Micro
 *
 * On first run, this should print something like:
 *
 *    42 0 0 0 42 0 0 0
 *    readWithCrc() failed
 *
 * because the initial EEPROM flash data (or the 'epoxypromdata' file under
 * EpoxyDuino) is filled with random data without a valid CRC.
 *
 * On the second run, this should print:
 *
 *    42 0 0 0 42 0 0 0
 *    readWithCrc() success!
 *    temp=42
 *
 * because the first run wrote the 'temp' variable with the correct CRC.
 */

#include <Arduino.h>
#include <EEPROM.h>
#include <AceUtilsCrcEeprom.h>
using ace_utils::crc_eeprom::CrcEeprom;

#ifndef SERIAL_PORT_MONITOR
#define SERIAL_PORT_MONITOR Serial
#endif

CrcEeprom crcEeprom;

void writeAndReadEeprom() {
  // Write at address=0
  uint32_t temp = 42;
  EEPROM.put(0, temp);

  // Write at the end of the EEPROM.
  EEPROM.write(1020, 42);
  EEPROM.write(1021, 0);
  EEPROM.write(1022, 0);
  EEPROM.write(1023, 0);

  SERIAL_PORT_MONITOR.println(EEPROM.read(0));
  SERIAL_PORT_MONITOR.println(EEPROM.read(1));
  SERIAL_PORT_MONITOR.println(EEPROM.read(2));
  SERIAL_PORT_MONITOR.println(EEPROM.read(3));

  SERIAL_PORT_MONITOR.println(EEPROM.read(1020));
  SERIAL_PORT_MONITOR.println(EEPROM.read(1021));
  SERIAL_PORT_MONITOR.println(EEPROM.read(1022));
  SERIAL_PORT_MONITOR.println(EEPROM.read(1023));
}

void writeAndReadCrcEeprom() {
  const int address = 512;
  crcEeprom.begin(1024);

  // Reading will fail the first time because the CRC won't match. Running this
  // program a second should succeed because the correct CRC will be written
  // by writeWithCrc() below.
  uint32_t temp;
  bool status = crcEeprom.readWithCrc(address, &temp, sizeof(temp));
  if (status) {
    SERIAL_PORT_MONITOR.println("readWithCrc() success!");
    SERIAL_PORT_MONITOR.print("temp=");
    SERIAL_PORT_MONITOR.println(temp);
  } else {
    SERIAL_PORT_MONITOR.println("readWithCrc() failed");
  }

  temp = 42;
  crcEeprom.writeWithCrc(address, &temp, sizeof(temp));
}

void setup() {
#if ! defined(EPOXY_DUINO)
  delay(1000); // some boards reboot twice
#endif
  SERIAL_PORT_MONITOR.begin(115200);
  while (!SERIAL_PORT_MONITOR); // For Leonardo/Micro

  writeAndReadEeprom();
  writeAndReadCrcEeprom();

#if defined(EPOXY_DUINO)
  exit(0);
#endif
}

void loop() {
}
