# EpoxyMockDigitalWriteFast Library

This is a simple mock for the `digitalWriteFast` library
(https://github.com/NicksonYap/digitalWriteFast) to allow code written against
that library to compile under EpoxyDuino. It provides stubs for the 3 functions
provided by the `digitalWriteFast` library:

* `digitalWriteFast()`
* `pinModeFast()`
* `digitalReadFast()`

## Usage

Since the `digitalWriteFast` library works only for AVR processors, we would
normally write some conditional code like this:

```C++
#if defined(ARDUINO_ARCH_AVR)
  #include <digitalWriteFast.h>
#endif

...

#define USE_DIGITAL_WRITE_FAST 0

void doSomething() {
  ...
  #if USE_DIGITAL_WRITE_FAST
    digitalWriteFast(PIN, LOW);
  #else
    digitalWrite(PIN, LOW);
  #endif
  ...
}
```

The problem is that this code will **not** compile under EpoxyDuino.

To fix that problem, change the code to look like this:

```C++
#if defined(ARDUINO_ARCH_AVR) || defined(EPOXY_DUINO)
  #include <digitalWriteFast.h>
#endif
...
```

Then add `EpoxyMockDigitalWriteFast` to the `ARDUINO_LIBS` in the EpoxyDuino
`Makefile`:

```make
APP_NAME := MyApp
ARDUINO_LIBS := EpoxyMockDigitalWriteFast ...
include ../../../../EpoxyDuino.mk
```
