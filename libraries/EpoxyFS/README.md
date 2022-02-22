# EpoxyFS

An implementation of the `FS` file system interface provided
[by ESP8266](https://arduino-esp8266.readthedocs.io/en/latest/filesystem.html)
and
[by ESP32](https://github.com/espressif/arduino-esp32/tree/master/libraries/FS)
that runs on a POSIX-like environment (Linux, MacOS, FreeBSD) using EpoxyDuino.

## Table of Contents

* [Usage](#Usage)
* [Examples](#Examples)
* [Portability](#Portability)
* [File System Location](#FileSystemLocation)
* [Bugs and Limitations](#BugsAndLimitations)

<a name="Usage"></a>
## Usage

The `FS` class in EpoxyFS is compatible with the `FS` class on the ESP8266 and
ESP32 platforms. There are several implementations of the `FS` class on various
platforms:

* ESP8266 v3.0.2
    * Built-in library
      [LittleFS](https://github.com/esp8266/Arduino/tree/master/libraries/LittleFS)
    * Built-in library
      [SPIFFS](https://github.com/esp8266/Arduino/tree/master/cores/esp8266/spiffs)
      which is
      [deprecated](https://arduino-esp8266.readthedocs.io/en/latest/filesystem.html#spiffs-deprecation-warning)
    * Built-in library
      [SDFS](https://github.com/esp8266/Arduino/tree/master/libraries/SDFS)
      (never tested by me)
* ESP32 v2.0.2
    * Built-in library
      [LittleFS](https://github.com/espressif/arduino-esp32/tree/master/libraries/LittleFS)
      which follows similar (but not always the same) conventions as the ESP8266
      LittleFS library.
    * Built-in library
      [SPIFFS](https://github.com/espressif/arduino-esp32/tree/master/libraries/SPIFFS)
      (Not sure if this is deprecated as well.)
    * Early versions of EpoxyFS tested against the
      [LITTLEFS](https://github.com/lorol/LITTLEFS) third party library.
      I'm not sure if this still works.

Each file system automatically creates a global instance of `FS` class for
convenience:

* `LittleFS.h` libraries create instances named `LittleFS`
* `LITTLEFS.h` library creates an instance named `LITTLEFS`
* `SPIFFS.h` libraries create instances named `SPIFFS`
* `SDFS.h` library creates a instance named `SDFS`
* `EpoxyFS.h` library creates an instance named `fs::EpoxyFS` (in the `fs`
  namespace to reduce name collision)

To write code that's compatible across multiple platforms, including EpoxyDuino,
I recommend using C-preprocessor macros to to set the correct file system
instance. In the following code snippet, the `#define FILE_SYSTEM` is set to the
appropriate instance of the selected file system library, and this is used in
all places where where a direct reference to `LittleFS`, or `LITTLEFS` or
`fs::EpoxyFS` instance would be made.

```C++
#include <Arduino.h>

#if defined(EPOXY_DUINO)
  #include <EpoxyFS.h>
  #define FILE_SYSTEM fs::EpoxyFS
#elif defined(ESP8266)
  #include <LittleFS.h>
  #define FILE_SYSTEM LittleFS
#elif defined(ESP32)
  #include <LittleFS.h>
  #define FILE_SYSTEM LittleFS
#else
  #error Unsupported platform
#endif

void doStuff(fs::File& file) {
  ...
}

void setup() {
  SERIAL_PORT_MONITOR.begin(115200);
  while (!SERIAL_PORT_MONITOR); // For Leonardo/Micro
  ...

  SERIAL_PORT_MONITOR.println(F("Initializing file system"));
  if (! FILE_SYSTEM.begin()){
    SERIAL_PORT_MONITOR.println(F("Error initializing file system"));
    // handle error
  }

  SERIAL_PORT_MONITOR.println(F("Opening myfile.txt"));
  fs::File file = FILE_SYSTEM.open("myfile.txt", "r");
  if (! file) {
    SERIAL_PORT_MONITOR.println(F("Error opening myfile.txt"));
    // handle error
  }

  doStuff(file);
  ...
}

void loop() {
  ...
}
```

<a name="Examples"></a>
## Examples

* [examples/HelloEpoxyFS](examples/HelloEpoxyFS)
    * Tested on EpoxyDuino, ESP8266 3.0.2 and ESP32 2.0.2.

<a name="Portability"></a>
## Portability

I have tested mostly against the "LittleFS" libraries on the ESP8266 and ESP32.

In my experience, the `fs::File` class is more compatible across different
platforms than the `fs::FS` class. So instead of passing around the `fs::FS`
instance, I had better luck obtaining portable code by passing around the
`fs::File` instance.

Some documentation (I cannot find it now) says that if the name of the file does
*not* begin with a '/' character, it is assumed to be at the root of the file
system tree. The `EpoxyFS` library follows that convention. However, I found
that in the most recent version of LittleFS on ESP32 v2.0.2, the '/' character
prefix was required to find the files that was uploaded through the [universal
LittleFS upload tool](https://github.com/lorol/arduino-esp32fs-plugin). For
portability, it may be necessary to always prepend a '/' to convert the file
name to an absolute path instead of a relative path .

The ESP8266 version of `File` implements `File::fullName()`. The ESP32 version
implements this as `file::path()`.

The ESP32 version of `LittleFS` does not implement the `fs::Dir` class. EpoxyFS
copied the API from ESP8266 and implements `fs::Dir`.

<a name="FileSystemLocation"></a>
## File System Location

By default, the root of the `EpoxyFS` file system is located in a directory
named `./epoxyfsdata` in the *current directory*. This location can be changed
by setting the `EPOXY_FS_ROOT` environment variable like this:

* `$ export EPOXY_FS_ROOT=/tmp/epoxyfsdata`

**DANGER**: The `EpoxyFS::format()` method creates a blank file system. To
achieve that, the method recursively removes all files and directories under the
`EPOXY_FS_ROOT` directory. If you set the `EPOXY_FS_ROOT` to an incorrect value,
you may remove a **lot** of files quickly and permanently. Be very careful!

You can add this snippet to your `.gitignore` file
```
# Root directory of EpoxyFS
epoxyfsdata
```
to prevent `git` from adding these files to the repo.

I believe both the ESP8266 and ESP32 upload tools assume that the data files
live in a directory called `data` in the sketch directory. I deliberately named
`epoxyfsdata` different from `data` to prevent unintended interaction with these
tools.

<a name="BugsAndLimitations"></a>
## Bugs and Limitations

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
