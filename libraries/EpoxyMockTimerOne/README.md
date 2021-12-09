# EpoxyMockTimerOne Library

A mock implementation of the `TimerOne` library
(https://github.com/PaulStoffregen/TimerOne) to allow code written against
that library to compile under EpoxyDuino.

Since TimerThree (https://github.com/PaulStoffregen/TimerThree) has the exact
same API, we can use this mock for TimerThree for mocking purposes as well.

## Usage

Code written against `TimerOne` should compile without change with this mock
library. All methods are stubbed out with empty function bodies.

```C++
#include <Arduino.h>
#include <TimerOne.h>
...

void doSomething() {
  ...
}

void setup() {
  Timer1.initialize(100); // do something every 100 micros
  Timer1.attachInterrupt(doSomething);
  ...
}

void loop() {
}
```

Then add `EpoxyMockTimerOne` to the `ARDUINO_LIBS` in the EpoxyDuino `Makefile`:

```make
APP_NAME := MyApp
ARDUINO_LIBS := EpoxyMockTimerOne ...
include ../../../../EpoxyDuino.mk
```
