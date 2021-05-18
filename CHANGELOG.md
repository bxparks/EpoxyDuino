# Changelog

* Unreleased
    * Add `EpoxyMockTimerOne` mock library for `TimerOne`
      (https://github.com/PaulStoffregen/TimerOne).
* 0.7 (2021-04-28)
    * Add `libraries/EpoxyMockDigitalWriteFast`, a simple mock library for
      `digitalWriteFast` (https://github.com/NicksonYap/digitalWriteFast) to
      allow code using these functions (e.g. `digitalWriteFast()`) to compile
      under EpoxyDuino.
    * Rename `EpoxyPromAvr` to `EpoxyEepromAvr`, and `EpoxyPromEsp` to
      `EpoxyEepromEsp`, for consistency and better self-description.
    * Add `EPOXY_CORE` and `EPOXY_CORE_PATH` variables to allow alternate
      Arduino Core files to be specified. Move current Core files from top-level
      into `./cores/epoxy/` subdirectory.
    * Add `memcpy_P()` and `vsnprintf_P()` (Thanks pmp-p@, #28).
    * Add `EXTRA_CXXFLAGS` to allow additional c++ flags to be specified.
    * Add `-Wextra` (in addition to `-Wall`) because some Arduino platforms
      enable both when "warnings" are enabled, so it's convenient to catch those
      warnings on the desktop when using EpoxyDuino.
* 0.6.2 (2021-03-15)
    * Add more stubs for Arduino API functions: `pulseIn()`, `pulseInLong()`,
      `shiftOut()`, `shiftIn()`, I2C and SPI pins (SS, MOSI, MISO, SCK, SDA,
      SCL), `analogWrite()`, typedefs for `boolean`, `byte`, `word`.
* 0.6.1 (2021-02-25)
    * Add experimental support for FreeBSD 12.2.
* 0.6 (2021-02-19)
    * Add `libraries/EpoxyFS`, an implementation of the ESP8266 `FS` file system
      that runs on Linux and MacOS.
    * Add `delayMicroSeconds()`, `WCharacter.h`, and stub implementations of
      `IPAddress.h`, `SPI.h`, by Erik Tideman (@ramboerik), see
      [PR #18](https://github.com/bxparks/EpoxyDuino/pull/18).
    * Add `libraries/EpoxyPromEsp` (EEPROM using the API of ESP8266/ESP32).
    * Add `libraries/EpoxyPromAvr` (EEPROM using the API of AVR processors).
* 0.5 (2021-01-21)
    * **Breaking Change** Change project name to "EpoxyDuino". The
      `UNIX_HOST_DUINO` macro is replaced with `EPOXY_DUINO`, and the
      `UnixHostDuino.mk` file is now `EpoxyDuino.mk`. The old macro and file are
      retained for backwards compatibility, but the GitHub repo name has
      changed. See
      [Issue #15](https://github.com/bxparks/UnixHostDuino/issues/15).
* 0.4 (2021-01-21)
    * Wrap `#ifdef` around `main()` to avoid conflict when compiling other
      Arduino cores with UnixHostDuino inside the `libraries/` directory.
    * Make `Stream::readBytes()` and `Stream::readString()` virtual, for
      compatibility with ESP8266, ESP32, STM32 and probably other 3rd party
      cores.
* 0.3.1 (2020-11-16)
    * Add support for `random()` function.
    * Cleanup header files.
* 0.3 (2020-08-20)
    * Add `#define` for various digital and analog pins: `D0`-`D19`, `A0`-`A9`.
    * Add stub for `analogRead()`.
    * Add `Print.printf()` provided by a significant number of Arduino boards
      (ESP8266, ESP32, Teensy, etc).
* 0.2.1 (2020-05-02)
    * Add `MORE_CLEAN`, an optional user-provided make-target to do additional
      cleaning, such as removing directories created by code-generators.
* 0.2 (2020-03-21)
    * Mark `main()` as a weak reference (supported by g++ and clang++) so that
      UniHostDuino can be used as a library in programs that already provide a
      `main()` function. (Thanks to Max Prokhorov mcspr@).
    * Replace `ARDUINO_LIB_DIR` with `ARDUINO_LIB_DIRS` which supports
      multiple external library locations, in addition to siblings of the
      UnixHostDuino directory. The `ARDUINO_LIB_DIR` is no longer supported.
* 0.1.3 (2019-11-21)
    * Add 'Installation' section to the README.md.
    * Add `UNIX_HOST_DUINO` macro symbol  to the CPPFLAGS to allow conditional
      code that activates only when UnixHostDuino is used.
* 0.1.2 (2019-09-04)
    * Implement `StdioSerial::flush()` to enable `Serial.flush()`.
* 0.1.1 (2019-08-14)
    * If the STDIN is not a real tty, continue without putting it into raw mode
      or exiting with an error. This allows unit tests inside continuous build
      frameworks.
* 0.1 (2019-07-31)
    * Split from `AUnit` and renamed from `unitduino` to `UnixHostDuino`.
    * Add `UNIT_HOST_DUINO` macro.
    * Add `FPSTR()` macro for compatibilty with ESP8266 and ESP32.
    * Add `LED_BUILTIN` macro.
