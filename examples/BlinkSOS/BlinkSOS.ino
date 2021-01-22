/*
 * Blink SOS - An Arduino program that runs on Linux and MacOS natively.
 * 
 * On Linux or Mac, type:
 *  * $ make
 *  * $ ./BlinkSOS.out
 *
 * Should print out the following:
 *
 *    LED_BUILTIN: 1
 *    LED: 1
 *    SOS
 *    SOS
 *    SOS
 *    ...
 */

#include <Arduino.h>

#define LED LED_BUILTIN
#define LED_ON HIGH
#define LED_OFF LOW

void setup() {
#if ! defined(EPOXY_DUINO)
  delay(1000); // some boards reboot twice
#endif
  SERIAL_PORT_MONITOR.begin(115200);
  while (!SERIAL_PORT_MONITOR); // For Leonardo/Micro

  SERIAL_PORT_MONITOR.print(F("LED_BUILTIN: "));
  SERIAL_PORT_MONITOR.println(LED_BUILTIN);
  SERIAL_PORT_MONITOR.print(F("LED: "));
  SERIAL_PORT_MONITOR.println(LED);

  pinMode(LED, OUTPUT);
  digitalWrite(LED, LED_OFF);
}

void longOn() {
  digitalWrite(LED, LED_ON);
  delay(500);
  digitalWrite(LED, LED_OFF);
  delay(200);
}

void shortOn() {
  digitalWrite(LED, LED_ON);
  delay(200);
  digitalWrite(LED, LED_OFF);
  delay(200);
}

void loop() {
  SERIAL_PORT_MONITOR.println(F("SOS"));

  shortOn();
  shortOn();
  shortOn();

  longOn();
  longOn();
  longOn();

  shortOn();
  shortOn();
  shortOn();

  delay(1000);
}
