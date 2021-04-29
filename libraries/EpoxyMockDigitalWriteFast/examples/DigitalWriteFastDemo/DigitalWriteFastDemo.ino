// This program requires the digitalWriteFast
// (https://github.com/NicksonYap/digitalWriteFast) library, which is supported
// only under AVR processors. But, we can verify compilation of this program
// using the EpoxyMockDigitalWriteFast mock library.
//
// Usually the primiary reason for using digitalWriteFast is for spee. However,
// the other reason for using digitalWriteFast is to save flash memory. This
// program uses 286 bytes with digitalWriteFast, which could matter in extreme
// memory constrained environments, like the ATtiny85 with only 8kB of flash.

#include <Arduino.h>
#include <digitalWriteFast.h>

#define USE_DIGITAL_WRITE_FAST 1

void setup() {
  #if USE_DIGITAL_WRITE_FAST
    pinModeFast(LED_BUILTIN, OUTPUT);
  #else
    pinMode(LED_BUILTIN, OUTPUT);
  #endif
}

void loop() {
  #if USE_DIGITAL_WRITE_FAST
    digitalWriteFast(LED_BUILTIN, LOW);
    delay(100);
    digitalWriteFast(LED_BUILTIN, HIGH);
    delay(100);
  #else
    digitalWrite(LED_BUILTIN, LOW);
    delay(300);
    digitalWrite(LED_BUILTIN, HIGH);
    delay(300);
  #endif
}
