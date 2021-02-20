# EpoxyFS

An implementation of the `FS` filesystem interface for ESP8266
(https://arduino-esp8266.readthedocs.io/en/latest/filesystem.html) that runs on
a Linux (and hopefully on a MacOS) machine using EpoxyDuino.

## Usage

The `FS` class in EpoxyFS is compatible with the `FS` class on the ESP8266
platform. Unfortunately, the `FS` class under the ESP32 is slightly different.
This means that we cannot use `FS` as a common interface across platforms.
Instead we use a `#define FILE_SYSTEM` to set the correct file system instance,
and use `FILE_SYSTEM` everywhere where we would have used a `LittleFS`, or
`LITTLEFS` or `fs::EpoxyFS` instance.

```C++
#include <Arduino.h>

#if defined(EPOXY_DUINO)
  #include <EpoxyFS.h>
  #define FILE_SYSTEM fs::EpoxyFS
#elif defined(ESP8266)
  #include <LittleFS.h>
  #define FILE_SYSTEM LittleFS
#elif defined(ESP32)
  #include <LITTLEFS.h>
  #define FILE_SYSTEM LITTLEFS
#else
  #error Unsupported platform
#endif

void doStuff() {
}
  
void setup() {
  SERIAL_PORT_MONITOR.begin(115200);
  while (!SERIAL_PORT_MONITOR); // For Leonardo/Micro
  ...

  if (FILE_SYSTEM.begin()){
    SERIAL_PORT_MONITOR.println(F("Filesystem initialized."));
  } else {
    SERIAL_PORT_MONITOR.println(F("Filesystem failed."));
  }
  ...
}

void loop() {
  ...
}
```

## Root of the File System

By default, the root of the file system is located in a directory named
`./epoxyfsdata` in the *current directory*. This location can be changed by
setting the

* `EPOXY_FS_ROOT`

environment variable. For example, setting

* `$ export EPOXY_FS_ROOT=/tmp/epoxyfsdata`

sets the root to the `/tmp/epoxyfsdata` directory.

**DANGER**: The `EpoxyFS::format()` method creates a blank file system. To
achieve that, the method recursively removes all files and directories under the
`EPOXY_FS_ROOT` directory. If you set the `EPOXY_FS_ROOT` to an incorrect value,
you may remove a **lot** of files quickly and permanently. Be very careful!

## Bugs and LImitation

* A number of functions and methods are not yet implemented. See the `TODO`
  notes in `src/EpoxyFS.h`.
* I have not tested all of the `FS::open()` modes: "r", "w", "a", "r+" etc.
* The document for [ESP8266
  Filesystem](https://arduino-esp8266.readthedocs.io/en/latest/filesystem.html)
  says that "Subdirectories are automatically created when you attempt to create
  a file in a subdirectory, and when the last file in a subdirectory is removed
  the subdirectory itself is automatically deleted. This is because there was no
  mkdir() method in the existing SPIFFS filesystem."
    * This has *not* been implemented in EpoxyFS.
