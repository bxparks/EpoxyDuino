# Changelog

* Unreleased
    * If the STDIN is not a real tty, don't bother putting it into raw mode,
      to allow unit tests inside continuous build frameworks.
* 0.1 (2019-07-31)
    * Split from `AUnit` and renamed from `unitduino` to `UnixHostDuino`.
    * Add `UNIT_HOST_DUINO` macro.
    * Add `FPSTR()` macro for compatibilty with ESP8266 and ESP32.
    * Add `LED_BUILTIN` macro.
