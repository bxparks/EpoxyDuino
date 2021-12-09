# EpoxyMockFastLED Library

A mock implementation of the [FastLED
library](https://github.com/FastLED/FastLED) to allow code written against that
library to compile under EpoxyDuino. This library implements just enough
functionality to allow the `ledstrips/LedController` application to compile.

## Usage

### Code Changes

Code written against `FastLED` should compile mostly without change with this
mock library. Most methods are stubbed out with empty function bodies.

```C++
#include <Arduino.h>
#include <FastLED.h>
...
```

The `FastLED.addLeds()` is a very complex, templatized, and overloaded set
of methods. They are not mocked out in this library. The code that calls
`addLeds()` should be conditionally commented out using something like:

```C++
#if ! defined(EPOXY_DUINO)
  FastLED.addLeds<LED_TYPE,LED_DATA_PIN,COLOR_ORDER>(leds, NUM_LEDS)
         .setCorrection(TypicalLEDStrip);
#endif
```

### Makefile

Add `EpoxyMockFastLED` to the `ARDUINO_LIBS` in the EpoxyDuino `Makefile`:

```make
APP_NAME := MyApp
ARDUINO_LIBS := EpoxyMockFastLED ...
include ../../../../EpoxyDuino.mk
```

