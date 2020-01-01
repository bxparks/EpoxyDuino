/*
 * Copyright (c) 2019 Brian T. Park
 * 
 * Parts derived from the Arduino SDK
 * Copyright (c) 2005-2013 Arduino Team
 * 
 * Parts inspired by [Entering raw
 * mode](https://viewsourcecode.org/snaptoken/kilo/02.enteringRawMode.html).
 *
 * Parts inspired by [ESP8266 Host
 * Emulation](https://github.com/esp8266/Arduino/tree/master/tests/host).
 *
 */

#include <inttypes.h>
#include <unistd.h> // usleep()
#include <time.h> // clock_gettime()
#include "Arduino.h"

// -----------------------------------------------------------------------
// Arduino methods emulated in Unix
// -----------------------------------------------------------------------

void delay(unsigned long ms) {
  usleep(ms * 1000);
}

void yield() {
  usleep(1000); // prevents program from consuming 100% CPU
}

unsigned long millis() {
  struct timespec spec;
  clock_gettime(CLOCK_MONOTONIC, &spec);
  unsigned long ms = spec.tv_sec * 1000U + spec.tv_nsec / 1000000UL;
  return ms;
}

unsigned long micros() {
  struct timespec spec;
  clock_gettime(CLOCK_MONOTONIC, &spec);
  unsigned long us = spec.tv_sec * 1000000UL + spec.tv_nsec / 1000U;
  return us;
}

void digitalWrite(uint8_t pin, uint8_t val) {}

int digitalRead(uint8_t pin) { return 0; }

void pinMode(uint8_t pin, uint8_t mode) {}

