# EpoxyEepromAvr

An implementation of `EEPROM` that runs on Linux (or other POSIX-like system)
using EpoxyDuino (https://github.com/bxparks/EpoxyDuino). There are at least two
versions of the `EEPROM` API:

* AVR flavor: AVR, STM32, Teensy, and potentially others
    * https://github.com/arduino/ArduinoCore-avr/blob/master/libraries/EEPROM/src/EEPROM.h
* ESP flavor: ESP8266 and ESP32,
    * https://github.com/esp8266/Arduino/blob/master/libraries/EEPROM/EEPROM.h

This library implements the **AVR** flavor of the EEPROM API. The EEPROM
contents are directly saved to a file called `epoxyeepromdata` in the current
directory. The AVR version of `EEPROM` does not provide a mechanism to set the
size of the EEPROM, since it is determined by the hardware. This library
hardcodes the size of its EEPROM emulation to 1024 bytes.

See [EpoxyEepromEsp](../EpoxyEepromEsp/) for the ESP version.

## Usage

### EEPROM API

This library provides most of the functions listed in
[EEPROM.h](https://github.com/arduino/ArduinoCore-avr/blob/master/libraries/EEPROM/src/EEPROM.h)
in the Arduino AVR Core.

### Makefile

When using the EpoxyDuino Makefile, the `EpoxyEepromAvr` library must be added to
the `ARDUINO_LIBS` parameter, like this:

```
APP_NAME := EpoxyEepromAvrTest
ARDUINO_LIBS := EpoxyEepromAvr ...
MORE_CLEAN := more_clean
include ../../../../EpoxyDuino.mk
```

### Sample Code

```C++
#include <EpoxyEepromAvr.h>
#define EEPROM EpoxyEepromAvrInstance

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
  #if EPOXY_DUINO_EPOXY_EEPROM_AVR
    #warning Using EpoxyEepromAvr version of EEPROM
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

### AVR Flavor

The AVR version provides 2 write functions:

```C++
EEPROM.write(address, value);
EEPROM.update(address, value);
```
where the `update()` only writes to the EEPROM if the new value is different
than the old value stored in the EEPROM. This saves wear on the EEPROM.

The `EEPROM.begin()` and `EEPROM.end()` methods return iterators, which allows
looping for the content of the EEPROM like this:

```C++
for (EEPtr p = EEPROM.begin(); p != EEPROM.end(); ++p) {
  ...
  uint8_t c = *p;
  ...
}
```

### ESP Flavor

The ESP8266 and ESP32 versions provide only the `write()` function:

```C++
EEPROM.write(address, value)
```

The `begin()` and `end()` methods mean completely different things on the ESP
processors:

```C++
EEPROM.begin(size);
```
initializes the EEPROM storage space. The data written to the EEPROM using
the `write()` method is buffered in memory until one of:

```C++
EEPROM.commit();
EEPROM.end();
```
is executed.

## Environment Variable

By default, the content of the `EEPROM` is saved to a file named
`epoxyeepromdata` in the current directory. You can change this by setting the
`EPOXY_EEPROM_DATA` environment variable:

```
$ export EPOXY_EEPROM_DATA=/tmp/epoxyeepromdata
```
