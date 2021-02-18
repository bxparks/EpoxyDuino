# EpoxyPromAvr

An implementation of `EEPROM` that runs on Linux (or other POSIX-like system)
using EpoxyDuino (https://github.com/bxparks/EpoxyDuino). There are two (maybe
more) versions of the `EEPROM` API:

* ESP8266 and ESP32,
* AVR, STM32 and others

This library implements the EEPROM API from the Arduino Cores for AVR (and
potentially others, e.g. STM32). See [EpoxyPromEsp](../EpoxyPromEsp/) for the
ESP8266/ESP32 version.

The EEPROM contents are directly saved to a file called `epoxypromdata` in the
current directory.

## Usage

### EEPROM API

This library provides most of the functions listed in
[EEPROM.h](https://github.com/arduino/ArduinoCore-avr/blob/master/libraries/EEPROM/src/EEPROM.h)
in the Arduino AVR Core.

### Makefile

When using the EpoxyDuino Makefile, the `EpoxyPromAvr` library must be added to
the `ARDUINO_LIBS` parameter, like this:

```
APP_NAME := EpoxyPromAvrTest
ARDUINO_LIBS := EpoxyPromAvr ...
MORE_CLEAN := more_clean
include ../../../../EpoxyDuino.mk
```

### Sample Code

```C++
#include <EEPROM.h> // global 'EEPROM' object defined

void write() {
  int address = 0;
  uint8_t value = 10;

  // write() method
  EEPROM.write(address, value);

  // operator[]
  EEPROM[address+1] = value+1;
}

void read() {
  int address = 0;

  // read()
  uint8_t value = EEPROM.read(address);

  // operator[]
  value = EEPROM[address];
}

void detectAPI() {
  #if EPOXY_DUINO_EPOXY_PROM_ESP
    #warning Using EpoxyPromEsp version of EEPROM
    EEPROM.begin(...);
    EEPROM.write(...);
    EEPROM.commit(...);
    EEPROM.read(...);
  #endif

  #if EPOXY_DUINO_EPOXY_PROM_AVR
    #warning Using EpoxyPromAvr version of EEPROM
    EEPROM.write(...);
    EEPROM.update(...);
    EEPROM.read(...);
  #endif
}

void setup() {
}

void loop() {
}
```

## Difference between ESP and AVR APIs

The AVR version of `EEPROM` provides 2 write functions:
```C++
EEPROM.write(address, value);
EEPROM.update(address, value);
```
where the `update()` only writes if the new value is different than the old
value stored in the EEPROM.

The ESP8266, ESP32 and EpoxyDuino version of `EEPROM` provides only the
`write()` function:
```C++
EEPROM.write(address, value)
```

Just like the ESP8266 version, the `EpoxyPromEsp` library requires:
```C++
EEPROM.begin(size);
```
to initialize the EEPROM storage space. And the content of the EEPROM
is stored in memory until
```C++
EEPROM.commit();
```
is executed.

## Environment Variable

By default, the content of the `EEPROM` is saved to a file named
`epoxypromdata` in the current directory. You can change this by setting the
`EPOXY_PROM_DATA` environment variable:

```
$ export EPOXY_PROM_DATA=/tmp/epoxypromdata
```
