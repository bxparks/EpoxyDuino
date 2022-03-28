# EpoxyMockSTM32RTC Library

This library provides a mock implementation of the `STM32RTC` class from the
[STM32RTC](https://github.com/stm32duino/STM32RTC) library. It provides just
enough functionality to run unit tests and a small demo. Only the date and time
functions are provided. The alarm functions are missing because AceTime does not
support alarms.

## Usage

Add `EpoxyMockSTM32RTC` to the `ARDUINO_LIBS` in the EpoxyDuino `Makefile` of
the application:

```make
APP_NAME := MyApp
ARDUINO_LIBS := AceCommon AceSorting AceTime AceTimeClock EpoxyMockSTM32RTC ...
include ../../../../EpoxyDuino.mk
```

In the code where `#include <STM32RTC.h>` appears, use the following instead:

```C++
#if defined(EPOXY_DUINO)
  #include <EpoxyMockSTM32RTC.h>
#else
  #include <STM32RTC.h>
#endif
```

The `EpoxyMockSTM32RTC.h` could have been named `STM32RTC.h` to avoid the `#if
defined()`, but I wanted to make it clear that `EpoxyMockSTM32RTC.h` does not
provide an *emulation* of the `STM32RTC.h` functionality. If the
`<EpoxyMockSTM32RTC.h>` implementation was hooked up to the Unix `time_t
time(nullptr)` function, then I think it would be appropriate to rename it to
just `<STM32RTC.h>`.
