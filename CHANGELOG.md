# Changelog

* Unreleased
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
