# EpoxyFS

An implementation of the `FS` filesystem interface for ESP8266
(https://arduino-esp8266.readthedocs.io/en/latest/filesystem.html) that runs on
a Linux (and hopefully on a MacOS) machine using EpoxyDuino.

## Usage

```C++
#include <Arduino.h>

#if defined(EPOXY_DUINO)
  #include <EpoxyFS.h>
  FS& FILE_SYSTEM = fs::EpoxyFS;
#elif defined(ESP8266)
  #include <LittleFS.h>
  using fs::LittleFS;
  FS& FILE_SYSTEM = fs::LittleFS;
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
`./fsdata` in the *current directory*. This location can be changed by setting
the

* `EPOXY_FS_ROOT`

environment variable. For example, setting

* `EPOXY_FS_ROOT=/tmp/fsdata`

sets the root to the `/tmp/fsdata` directory.

## Bugs and LImitation

* A number of functions and methods are not yet implemented. See the `TODO`
  notes in `src/EpoxyFS.h`.
* I have not tested all of the `FS::open()` modes: "r", "w", "a", "r+" etc.
