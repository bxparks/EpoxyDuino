# EpoxyDuino

[![AUnit Tests](https://github.com/bxparks/EpoxyDuino/actions/workflows/aunit_tests.yml/badge.svg)](https://github.com/bxparks/EpoxyDuino/actions/workflows/aunit_tests.yml)

**New**: [GitHub Discussions](https://github.com/bxparks/EpoxyDuino/discussions)
for this project is now active! Let's use that for general support questions,
and reserve the [GitHub Issues](https://github.com/bxparks/EpoxyDuino/issues)
section for bugs and feature requests.

This project contains a small (but often effective) implementation of the
Arduino programming framework for Linux, MacOS, FreeBSD (experimental) and
potentially other POSIX-like systems. Originally, it was created to allow
[AUnit](https://github.com/bxparks/AUnit) unit tests to be compiled and run on a
desktop class machine, instead of running on the embedded microcontroller. As
more Arduino functionality was added, I found it useful for doing certain types
of application development on my Linux laptop, especially the parts that were
more algorithmic instead of hardware dependent. EpoxyDuino can be effectively
used in Continuous Integration (CI) pipeline (like
[GitHub Actions](https://github.com/features/actions)) for automatically
validating that a library or application compiles without errors.

The build process uses [GNU Make](https://www.gnu.org/software/make/manual/).
A simple `Makefile` needs to be created inside the sketch folder. For example,
if the sketch is `SampleTest/SampleTest.ino`, then the makefile should be
`SampleTest/Makefile`. The sketch is compiled with just a `make` command. It
produces an executable with a `.out` extension, for example, `SampleTest.out`.

Most hardware dependent functions are stubbed out (defined but don't do
anything) to allow the Arduino programs to compile. This may be sufficient for a
CI pipeline. For actual application development, I have started to build
a set of libraries within EpoxyDuino which emulate the versions that run the
actual hardware:

* EpoxyFS: emulation of the ESP8266 LittleFS or ESP32 LITTLEFS
* EpoxyEepromAvr: emulation of AVR-flavored `EEPROM`
* EpoxyEepromEsp: emulation of ESP-flavored `EEPROM`

If your program has limited hardware dependencies so that it is conceptually
portable to a vanilla Unix environment, EpoxyDuino may work well for you.

Running an Arduino program natively on a desktop-class machine has some
advantages:

* The development cycle can be lot faster because the compilers on the the
  desktop machines are a lot faster, and we also avoid the upload and flash
  process to the microcontroller.
* The desktop machine can run unit tests which require too much flash or too
  much memory to fit inside an embedded microcontroller.
* It may help you write platform-independent code, because if it runs under
  EpoxyDuino, it has a good chance of running on most Arduino-compatible
  platforms.

The disadvantages are:

* Only a subset of Arduino functions are supported (see below).
* Many 3rd party libraries will not compile under EpoxyDuino.
* There may be compiler differences between the desktop and the embedded
  environments (e.g. 16-bit `int` versus 32-bit `int`, or 32-bit `long` versus
  64-bit `long`).

**Version**: 0.7 (2021-04-28)

**Changelog**: See [CHANGELOG.md](CHANGELOG.md)

**Breaking Change**: Prior to v0.5, this project was known as "UnixHostDuino".
The old `UNIX_HOST_DUINO` macro and `UnixHostDuino.mk` include file still exist
for backwards compatibility. See
[Issue #15](https://github.com/bxparks/EpoxyDuino/issues/15)
for more details.

## Table of Contents

* [Installation](#Installation)
* [Usage](#Usage)
    * [Makefile](#Makefile)
    * [Additional Arduino Libraries](#AdditionalLibraries)
    * [Additional Arduino Library Locations](#AdditionalLibraryLocations)
    * [Difference from Arduino IDE](#DifferenceFromArduinoIDE)
    * [Conditional Code](#ConditionalCode)
    * [Managing Multiple Makefiles](#ManagingMultipleMakefiles)
    * [Continuous Integration](#ContinuousIntegration)
* [Advanced Usage](#AdvancedUsage)
    * [Alternate C++ Compiler](#AlternateCompiler)
    * [Generated Source Code](#GeneratedSourceCode)
    * [Additional Clean Up](#AdditionalCleanUp)
    * [Alternate Arduino Core](#AlternateArduinoCore)
* [Supported Arduino Features](#SupportedArduinoFeatures)
    * [Arduino Functions](#ArduinoFunctions)
    * [Serial Port Emulation](#SerialPortEmulation)
* [Libraries and Mocks](#LibrariesAndMocks)
    * [Inherently Compatible Libraries](#InherentlyCompatibleLibraries)
    * [Emulation Libraries](#EmulationLibraries)
    * [Mock Libraries](#MockLibraries)
* [System Requirements](#SystemRequirements)
* [License](#License)
* [Feedback and Support](#FeedbackAndSupport)
* [Authors](#Authors)

<a name="Installation"></a>
## Installation

You need to grab the sources directly from GitHub. This project is *not* an
Arduino library so it is not available through the [Arduino Library
Manager](https://www.arduino.cc/en/guide/libraries) in the Arduino IDE.

The location of the EpoxyDuino directory can be arbitrary, but a convenient
location might be the same `./libraries/` directory used by the Arduino IDE to
store other Arduino libraries:

```
$ cd {sketchbook_directory}/libraries
$ git clone https://github.com/bxparks/EpoxyDuino.git
```

This will create a directory called
`{sketchbook_directory}/libraries/EpoxyDuino`.

### Dependencies

The core of EpoxyDuino depends on:

* a C++ compiler (`g++` or `clang++`)
* GNU Make (usually `make` but sometimes `gmake`)

These are normally installed on the host OS by default.

The example and test code under `./tests/`, `./examples/`,
`./libraries/*/tests/`, and `./libraries/*/examples/` depend on:

* AUnit (https://github.com/bxparks/AUnit)
* AceCRC (https://github.com/bxparks/AceCRC)
* AceCommon (https://github.com/bxparks/AceCommon)
* AceRoutine (https://github.com/bxparks/AceRoutine)
* AceUtils (https://github.com/bxparks/AceUtils)

<a name="Usage"></a>
## Usage

<a name="Makefile"></a>
### Makefile

The minimal `Makefile` has 3 lines:
```
APP_NAME := {name of project}
ARDUINO_LIBS := {list of dependent Arduino libraries}
include {path/to/EpoxyDuino.mk}
```

For example, the [examples/BlinkSOS](examples/BlinkSOS) project contains this
Makefile:
```
APP_NAME := BlinkSOS
ARDUINO_LIBS :=
include ../../../EpoxyDuino/EpoxyDuino.mk
```

To build the program, just run `make`:
```
$ cd examples/BlinkSOS
$ make clean
$ make
```

The executable will be created with a `.out` extension. To run it, just type:
```
$ ./BlinkSOS.out
```

The output that would normally be printed on the `Serial` on an Arduino
board will be sent to the `STDOUT` of the Linux, MacOS, or FreeBSD terminal. The
output should be identical to what would be shown on the serial port of the
Arduino controller.

<a name="AdditionalLibraries"></a>
### Additional Arduino Libraries

If the Arduino program depends on additional Arduino libraries, they must be
specified in the `Makefile` using the `ARDUINO_LIBS` parameter. For example,
this includes the [AUnit](https://github.com/bxparks/AUnit) library if it is at
the same level as EpoxyDuino:

```
APP_NAME := SampleTest
ARDUINO_LIBS := AUnit AceButton AceTime
include ../../EpoxyDuino/EpoxyDuino.mk
```

The libraries are referred to by their base directory name (e.g. `AceButton`,
or `AceTime`) not the full path. By default, the `EpoxyDuino.mk` file will look
for these additional libraries at the following locations:

* `EPOXY_DUINO_DIR/..` - in other words, siblings to the `EpoxyDuino` install
  directory (this assumes that EpoxyDuino was installed in the Arduino
  `libraries` directory as recommended above)
* `EPOXY_DUINO_DIR/libraries/` - additional libraries provided by the EpoxyDuino
  project itself
* under each of the additional directories listed in `ARDUINO_LIB_DIRS` (see
  below)

<a name="AdditionalLibraryLocations"></a>
### Additional Arduino Library Locations

As explained above, EpoxyDuino normally assumes that the additional libraries
are siblings to the`EpoxyDuino/` directory or under the `EpoxyDuino/libraries/`
directory. If you need to import additional Arduino libraries, you need to tell
`EpoxyDuino` where they are because Arduino libraries tend to be scattered among
many different locations. These additional locations can be specified using the
`ARDUINO_LIB_DIRS` variable. For example,

```
APP_NAME := SampleTest
arduino_ide_dir := ../../arduino-1.8.9
ARDUINO_LIBS := AUnit AceButton AceTime
ARDUINO_LIB_DIRS := \
	$(arduino_ide_dir)/portable/packages/arduino/hardware/avr/1.8.2/libraries \
	$(arduino_ide_dir)/libraries \
	$(arduino_ide_dir)/hardware/arduino/avr/libraries
include ../../EpoxyDuino/EpoxyDuino.mk
```

Each of the `AUnit`, `AceButton` and `AceTime` libraries will be searched in
each of the 3 directories given in the `ARDUINO_LIB_DIRS`. (The
`arduino_ide_dir` is a convenience temporary variable. It has no significance to
`EpoxyDuino.mk`)

<a name="DifferenceFromArduinoIDE"></a>
### Difference from Arduino IDE

There are a number of differences compared to the programming environment
provided by the Arduino IDE:

* The `*.ino` file is treated like a normal `*.cpp` file. So it must have
  an `#include <Arduino.h>` include line at the top of the file. This is
  compatible with the Arduino IDE which automatically includes `<Arduino.h>`.
* The Arduino IDE supports multiple `ino` files in the same directory. (I
  believe it simply concontenates them all into a single file.) EpoxyDuino
  supports only one `ino` file in a given directory.
* The Arduino IDE automatically generates forward declarations for functions
  that appear *after* the global `setup()` and `loop()` methods. In a normal
  C++ file, these forward declarations must be created by hand. The other
  alternative is to move `loop()` and `setup()` functions to the end of the
  `ino` file.

Fortunately, the changes required to make an `ino` file compatible with
EpoxyDuino are backwards compatible with the Arduino IDE. In other words, a
program that compiles with EpoxyDuino will also compile under Ardunio IDE.

There are other substantial differences. The Arduino IDE supports multiple
microcontroller board types, each using its own set of compiler tools and
library locations. There is a complicated set of files and rules that determine
how to find and use those tools and libraries. The EpoxyDuino tool does *not*
use any of the configuration files used by the Arduino IDE. Sometimes, you can
use the `ARDUINO_LIB_DIRS` to get around this limitations. However, when you
start using `ARDUINO_LIB_DIRS`, you will often run into third party libraries
using features which are not supported by the EpoxyDuino framework emulation
layer.

<a name="ConditionalCode"></a>
### Conditional Code

If you want to add code that takes effect only on EpoxyDuino, you can use
the following macro:
```C++
#if defined(EPOXY_DUINO)
  ...
#endif
```

If you need to target a particular desktop OS, you can use the following:

**Linux**:
```C++
#if defined(__linux__)
  ...
#endif
```

**MacOS**:
```C++
#if defined(__APPLE__)
  ...
#endif
```

**FreeBSD**:
```C++
#if defined(__FreeBSD__)
  ...
#endif
```

<a name="ManagingMultipleMakefiles"></a>
### Managing Multiple Makefiles

It is often convenient to create a parent Makefile that runs multiple targets in
the Makefiles under the subdirectories. For example, most of my libraries have
the following directory structure:

```
FooLibrary
|-- LICENSE
|-- README.md
|-- examples
|   |-- ExampleA
|   |   |-- ExampleA.ino
|   |   `-- Makefile
|   |-- ExampleB
|   |   |-- ExampleB.ino
|   |   `-- Makefile
|   |-- ...
|   |-- ExampleN
|   |   |-- ExampleN.ino
|   |   `-- Makefile
|   `-- Makefile
|-- library.properties
|-- src
|   |-- FooLibrary.h
|   |-- foolib
|   |   |-- file.h
|   |   `-- file.cpp
`-- tests
    |-- AxxTest
    |   |-- AxxTest.ino
    |   `-- Makefile
    |-- BxxTest
    |   |-- BxxTest.ino
    |   `-- Makefile
    |-- ...
    |-- MxxTest
    |   |-- MxxTest.ino
    |   `-- Makefile
    `-- Makefile
```

I often want to compile **all** of the examples , and run **all** the unit tests
with a single command. There are multiple ways to do this, but the technique
that I use is to create a parent `Makefile` in the `examples/` and `tests/`
directories that recursively runs the targets of the subdirectories. In
`examples/Makefile`, I create the following:

```make
all:
	set -e; \
	for i in */Makefile; do \
		echo '==== Making:' $$(dirname $$i); \
		$(MAKE) -C $$(dirname $$i) -j; \
	done

clean:
	set -e; \
	for i in */Makefile; do \
		echo '==== Cleaning:' $$(dirname $$i); \
		$(MAKE) -C $$(dirname $$i) clean; \
	done
```

In `tests/Makefile`, I create the following:

```make
tests:
	set -e; \
	for i in *Test/Makefile; do \
		echo '==== Making:' $$(dirname $$i); \
		$(MAKE) -C $$(dirname $$i) -j; \
	done

runtests:
	set -e; \
	for i in *Test/Makefile; do \
		echo '==== Running:' $$(dirname $$i); \
		$$(dirname $$i)/$$(dirname $$i).out; \
	done

clean:
	set -e; \
	for i in *Test/Makefile; do \
		echo '==== Cleaning:' $$(dirname $$i); \
		$(MAKE) -C $$(dirname $$i) clean; \
	done
```

To compile and run all the examples and unit tests, I do the following:

```bash
$ make -C examples clean
$ make -C tests clean

$ make -C examples all
$ make -C tests tests
$ make -C tests runtests | grep failed
```

These parent Makefiles can be used in Continuous Integration, as shown below.

<a name="ContinuousIntegration"></a>
### Continuous Integration

You can use EpoxyDuino to run continuous integration tests or
validations on the [GitHub Actions](https://github.com/features/actions)
infrastructure. The basic `ubuntu-18.04` docker image already contains the C++
compiler and `make` binary. You don't need to install the Arduino IDE or the
Arduino CLI. You need:

* EpoxyDuino,
* your project that you want to test,
* any additional Arduino libraries that you use.

Take a look at some of my GitHub Actions YAML config files:

* https://github.com/bxparks/AceButton/tree/develop/.github/workflows
* https://github.com/bxparks/AceCRC/tree/develop/.github/workflows
* https://github.com/bxparks/AceCommon/tree/develop/.github/workflows
* https://github.com/bxparks/AceRoutine/tree/develop/.github/workflows
* https://github.com/bxparks/AceTime/tree/develop/.github/workflows

<a name="AdvancedUsage"></a>
## Advanced Usage

<a name="AlternateCompiler"></a>
### Alternate C++ Compiler

Normally the C++ compiler on Linux is `g++`. If you have `clang++` installed
you can use that instead by specifying the `CXX` environment variable:
```
$ CXX=clang++ make
```
(This sets the `CXX` shell environment variable temporarily, for the duration of
the `make` command, which causes `make` to set its internal `CXX` variable,
which causes `EpoxyDuino.mk` to use `clang++` over the default `g++`.)

<a name="GeneratedSourceCode"></a>
### Generated Source Code

If a source file is generated dynamically through a code generation script,
and the source file is *not* checked into the repository because it is too
dynamic, then you can include the generated files using the `GENERATED`
and the `OBJS` variables.

First add the list of generated files `*.cpp` or `*.c` to the `GENERATED`
variable. Then add the corresponding `*.o` files to the `OBJS` variable, like
this:

```
GENERATED := foo.cpp bar.cpp
OBJS := foo.o bar.o
APP_NAME := {name of project}
ARDUINO_LIBS := {list of dependent Arduino libraries}
include {path/to/EpoxyDuino.mk}

foo.cpp: foo.h generate_foo.sh
    ./generate_foo.sh # creates 'foo.cpp'

bar.cpp: bar.h generate_bar.sh
    ./generate_bar.sh # creates 'bar.cpp'

...
```

The `*.o` files in `OJBS` are passed to the linker when the `app.out` binary
file is created.

The `GENERATED` is not absolutely required, since the default rules already know
how to compile the `*.o` files from the `*.cpp` or `*.c` files. The primary
effect of `GENERATED` currently is to cause the generated files to be removed
when `make clean` is called.

<a name="AdditionalCleanUp"></a>
### Additional Clean Up

The `make clean` rule is predefined to remove all of the intermediate `*.o`
files and `GENERATED` files that the `EpoxyDuino.mk` file knows about.
Sometimes, we want to do additional clean up. For example, the EEPROM emulation
libraries ([EpoxyEepromAvr](libraries/EpoxyEepromAvr) or
(EpoxyEepromEsp)[libraries/EpoxyEepromEsp]) will create a file in the current
directory named `epoxyeepromdata` which stores the content of the emulated
`EEPROM`. To remove such extra files, we can create a new `Makefile` target that
performs the clean up, and add the name of the target to `MORE_CLEAN`.

For example, we create a target named `more_clean` to perform the extra clean
up, and tell the `clean` target to depend on `more_clean` target using the
`MORE_CLEAN` variable:

```
MORE_CLEAN := more_clean
APP_NAME := {name of project}
ARDUINO_LIBS := {list of dependent Arduino libraries}
include {path/to/EpoxyDuino.mk}

more_clean:
    rm -f epoxyeepromdata
```

<a name="AlternateArduinoCore"></a>
### Alternate Arduino Core

This is very advanced. The Arduino ecosystem supports different hardware
processors, architectures, and platforms. The software environment for a
specific hardware environment is called a Core. The environment provided by
EpoxyDuino resembles the AVR Core most closely because a lot of the API
emulation code was borrowed from the AVR Core. However, EpoxyDuino does not
provide an exact emulation of the AVR Core. In fact, I consider EpoxyDuino to be
its own unique version of the Arduino API.

There may be situations where an Arduino program is specifically meant to run
under a hardware platform other than an AVR processor. If we want to use
EpoxyDuino to compile that program under Linux/MacOS/FreeBSD, we must provide a
different Arduino Core API. For example, if your program is meant to run on an
ESP8266 or ESP32 using its WiFi network capabilities, you may need special APIs
to compile that program under EpoxyDuino.

EpoxyDuino provides the ability substitute a different Arduino API Core through
2 Makefile variables:

* `EPOXY_CORE`
    * Use the core defined in the subdirectory under
      `$(EPOXY_DUINO_DIR)/cores/`.
    * By default, this variable defined to be `epoxy`, so
      the core files are searched under `$(EPOXY_DUINO_DIR)/cores/epoxy/`.
    * Currently `epoxy` is the only Core provided by the EpoxyDuino package.
* `EPOXY_CORE_PATH`
    * Defines the full-path to the Arduino Core API files.
    * If not overridden by the provided Makefile, this is set to
      `$(EPOXY_DUINO_DIR)/cores/$(EPOXY_CORE)`.

<a name="SupportedArduinoFeatures"></a>
## Supported Arduino Features

<a name="ArduinoFunctions"></a>
### Arduino Functions

The following functions and features of the Arduino framework are implemented:

* `Arduino.h`
    * `setup()`, `loop()`
    * `delay()`, `yield()`, `delayMicroSeconds()`
    * `millis()`, `micros()`
    * `digitalWrite()`, `digitalRead()`, `pinMode()` (empty stubs)
    * `analogRead()`, `analogWrite()` (empty stubs)
    * `pulseIn()`, `pulseInLong()`, `shiftIn()`, `shiftOut()` (empty stubs)
    * `HIGH`, `LOW`, `INPUT`, `OUTPUT`, `INPUT_PULLUP`
    * I2C and SPI pins: `SS`, `MOSI`, `MISO`, `SCK`, `SDA`, `SCL`
    * typedefs: `boolean`, `byte`, `word`
* `StdioSerial.h`
    * `Serial.print()`, `Serial.println()`, `Serial.write()`
    * `Serial.read()`, `Serial.peek()`, `Serial.available()`
    * `SERIAL_PORT_MONITOR`
* `WString.h`
    * `class String`
    * `class __FlashStringHelper`, `F()`, `FPSTR()`
* `Print.h`
    * `class Print`, `class Printable`
    * `Print.printf()` - extended function supported by some Arduino compatible
      microcontrollers
* `pgmspace.h`
    * `pgm_read_byte()`, `pgm_read_word()`, `pgm_read_dword()`,
      `pgm_read_float()`, `pgm_read_ptr()`
    * `strlen_P()`, `strcat_P()`, `strcpy_P()`, `strncpy_P()`, `strcmp_P()`,
      `strncmp_P()`, `strcasecmp_P()`, `strchr_P()`, `strrchr_P()`
    * `memcpy_P()`, `vsnprintf_P()`
    * `PROGMEM`, `PGM_P`, `PGM_VOID_P`, `PSTR()`
* `IPAddress.h`
    * `IPAddress` class
* `WCharacter.h`
    * `isAlpha()`, `isAscii()`, etc.
    * `toLowerCase(), `toUpperCase()`, etc
* `Wire.h` (stub implementation)
* `SPI.h` (stub implementation)

See [Arduino.h](https://github.com/bxparks/EpoxyDuino/blob/develop/Arduino.h)
for the latest list. Most of the header files included by this `Arduino.h`
file were copied and modified from the [arduino:avr
core](https://github.com/arduino/ArduinoCore-avr/tree/master/cores/arduino),
versions 1.8.2 (if I recall) or 1.8.3. A number of tweaks have been made to
support slight variations in the API of other platforms, particularly the
ESP8266 and ESP32 cores.

The `Print.printf()` function is an extension to the `Print` class that is
provided by many Arduino-compatible microcontrollers (but not the AVR
controllers). It is implemented here for convenience. The size of the internal
buffer is `250` characters, which can be changed by changing the
`PRINTF_BUFFER_SIZE` parameter if needed.

<a name="SerialPortEmulation"></a>
### Serial Port Emulation

The `Serial` object is an instance of the `StdioSerial` class which emulates the
Serial port using the `STDIN` and `STDOUT` of the Unix system. `Serial.print()`
sends the output to the `STDOUT` and `Serial.read()` reads from the `STDIN`.

The interaction with the Unix `tty` device is complicated, and I am not entirely
sure that I have implemented things properly. See [Entering raw
mode](https://viewsourcecode.org/snaptoken/kilo/02.enteringRawMode.html) for
in-depth details. The following is a quick summary of how this is implemented
under `EpoxyDuino`.

The `STDOUT` remains mostly in normal mode. In particular, `ONLCR` mode is
enabled, which translates `\n` (NL) to `\r\n` (CR-NL). This allows the program
to print a line of string terminating in just `\n` (e.g. in a `printf()`
function) and the Unix `tty` device will automatically add the `\r` (CR) to
start the next line at the left. (Interestingly, the `Print.println()` method
prints `\r\n`, which gets translated into `\r\r\n` by the terminal, which still
does the correct thing. The extra `\r` does not do any harm.)

The `STDIN` is put into "raw" mode to avoid blocking the `loop()` function while
waiting for input from the keyboard. It also allows `ICRNL` and `INLCR` which
flips the mapping of `\r` and `\n` from the keyboard. That's because normally,
the "Enter" or "Return" key transmits a `\r`, but internally, most string
processing code wants to see a line terminated by `\n` instead. This is
convenient because when the `\n` is printed back to the screen, it becomes
translated into `\r\n`, which is what most people expect is the correct
behavior.

The `ISIG` option on the `tty` device is *enabled*. This allows the usual Unix
signals to be active, such as Ctrl-C to quit the program, or Ctrl-Z to suspend
the program. But this convenience means that the Arduino program running under
`EpoxyDuino` will never receive a control character through the
`Serial.read()` function. The advantages of having normal Unix signals seemed
worth the trade-off.

<a name="LibrariesAndMocks"></a>
## Libraries and Mocks

The Arduino ecosystem provides thousands of libraries that can be used to extend
the functionality of an Arduino application. Some of these libraries will
work perfectly fine with EpoxyDuino, some will not. It is difficult to
categorize these libraries in a sensible way in the context of EpoxyDuino, but
here is my current attempt:

* **Inherently Compatible Libraries**:
    * Mostly algorithmic or have limited dependency on low-level Arduino API
      (e.g. `millis()`, `micros()`, `delay()`, `F()`).
    * If these have been written to be cross-platform across different Arduino
      hardware, then these should also automatically work under EpoxyDuino
      without much (or any) modifications.
* **Emulation Libraries**.
    * Libraries for EpoxyDuino written specifically to emulate the
      functionality of an Arduino library, for example, using the filesystem or
      network layer.
* **Mock or Stub Libraries**
    * Libraries which implement the API of the target library, but don't
      implement the functionality of the library.
    * These are useful for Continuous Integration workflows to verify that a
      program or library compiles with EpoxyDuino.
    * The assumption is that if something compiles under EpoxyDuino, it probably
      compiles under an actual Arduino environment.

These 3 types are described in more detail below.

<a name="InherentlyCompatibleLibraries"></a>
### Inherently Compatible Libraries

Almost all libraries that I write will be inherently compatible with EpoxyDuino
because EpoxyDuino is what I use to my libraries.

* AUnit (https://github.com/bxparks/AUnit)
* AceButton (https://github.com/bxparks/AceButton)
* AceCRC (https://github.com/bxparks/AceCRC)
* AceCommon (https://github.com/bxparks/AceCommon)
* AceRoutine (https://github.com/bxparks/AceRoutine)
* AceTime (https://github.com/bxparks/AceTime)
* AceUtils (https://github.com/bxparks/AceUtils), mostly

There are probably many other 3rd party libraries which are inherently
compatible with EpoxyDuino but we won't know until we try to compile them under
EpoxyDuino. If there are compile-time problems, it may be possible that only a
small set of tweaks are required to make it work. Often, the fixes are similar
to the changes needed to make the library cross-compatible with other Arduino
platforms.

<a name="EmulationLibraries"></a>
### Emulation Libraries

These libraries are designed partially or fully emulate the functionality a
particular Arduino library in the Unix-like desktop environment using
EpoxyDuino. I have provide provide 3 such libraries within the EpoxyDuino
project:

* [libraries/EpoxyFS](libraries/EpoxyFS)
    * An implementation of a file system compatible with
      [ESP8266 LittleFS](https://arduino-esp8266.readthedocs.io/en/latest/filesystem.html)
      and [ESP32 LITTLEFS](https://github.com/lorol/LITTLEFS).
* Two EEPROM implementations:
    * [libraries/EpoxyEepromAvr](libraries/EpoxyEepromAvr)
        * API compatible with
          [EEPROM on AVR](https://github.com/arduino/ArduinoCore-avr/tree/master/libraries/EEPROM)
    * [libraries/EpoxyEepromEsp](libraries/EpoxyEepromEsp)
        * API compatible with
          [EEPROM on ESP8266](https://github.com/esp8266/Arduino/tree/master/libraries/EEPROM)
          and
          [EEPROM on ESP32](https://github.com/espressif/arduino-esp32/tree/master/libraries/EEPROM)

Since the desktop environment already has a working network stack, I hope to
make create additional network libraries (HTTP client, HTTP Server, MQTT client,
etc) for EpoxyDuino, so that even more of the Arduino development can be done on
the Linux/MacOS host.

<a name="MockLibraries"></a>
### Mock Libraries

Mock libraries are designed to run under EpoxyDuino and provide non-working API
stubs of the target library. These libraries are useful to verify that a program
compiles, but they do not allow us to actually verify that the library works as
intended. This limitation may be sufficient for Continous Integration purposes.

* Wire
    * The `Wire.h` header file is provided automatically by the `<Arduino.h>`
      file in EpoxyDuino. No additional library needs to be added to the
      `ARDUINO_LIBS` variable in the `Makefile`.
    * It provides only mock functions of the actualy `Wire` library that
      is provided by real Arduino frameworks.
    * This was added very early in the development of EpoxyDuino so that I could
      compile some of my programs. I don't think I realized at the time that
      `Wire` is a separate (but built-in) library. In retrospect, it may have
      been better to split this file into a separate mock library.
* SPI
    * The `SPI.h` header file was contributed recently (see #18 and #19) and
      is included automatically by the `<Arduino.h>` file in EpoxyDuino.
    * It follows the same pattern as `Wire`, the header file provides only mock
      functions of the actual `SPI` library.
* [libaries/EpoxyMockDigitalWriteFast](libraries/EpoxyMockDigitalWriteFast)
    * A simple mock for the `digitalWriteFast` library
      (https://github.com/NicksonYap/digitalWriteFast) to allow code written
      against it to compile under EpoxyDuino.
* EspMock (https://github.com/hsaturn/EspMock)
    * This is a separate project that provides various mocks for functions and
      libraries included with the ESP8266 and the ESP32 processors.
    * It is not an Arduino library, so it needs to be installed using a manual
      `git clone`.

<a name="SystemRequirements"></a>
## System Requirements

This library has been tested on:

* Ubuntu 18.04
    * g++ (Ubuntu 7.5.0-3ubuntu1~18.04) 7.5.0
    * clang++ 8.0.0-3~ubuntu18.04.2
    * clang++ 6.0.0-1ubuntu2
    * GNU Make 4.1
* Ubuntu 20.04
    * g++ (Ubuntu 9.3.0-10ubuntu2) 9.3.0
    * clang++ version 10.0.0-4ubuntu1
    * GNU Make 4.2.1
* Raspbian GNU/Linux 10 (buster)
    * On Raspberry Pi Model 3B
    * g++ (Raspbian 8.3.0-6+rpi1) 8.3.0
    * GNU Make 4.2.1
* MacOS 10.14.5 (Mojave)
    * clang++ Apple LLVM version 10.0.1
    * GNU Make 3.81
* MacOS 10.14.6 (Mojave)
    * Apple clang version 11.0.0 (clang-1100.0.33.17)
    * GNU Make 3.81
* FreeBSD 12.2
    * c++: FreeBSD clang version 10.0.1
    * gmake: GNU Make 4.3
        * Install using `$ pkg install gmake`
        * You can type `gmake` instead of `make`, or
        * Create a shell alias, or
        * Create a symlink in `~/bin`.

<a name="License"></a>
## License

[MIT License](https://opensource.org/licenses/MIT)

<a name="Bugs"></a>
## Bugs and Limitations

If the executable (e.g. `SampleTest.out`) is piped to the `less(1)` or `more(1)`
command, sometimes (not all the time) the executable hangs and displays nothing
on the pager program. I don't know why, it probably has to do with the way that
the `less` or `more` programs manipulate the `stdin`. The solution is to
explicitly redirect the `stdin`:

```
$ ./SampleTest.out | grep failed # works

$ ./SampleTest.out | less # hangs

$ ./SampleTest.out < /dev/null | less # works
```

<a name="FeedbackAndSupport"></a>
## Feedback and Support

If you find this library useful, consider starring this project on GitHub. The
stars will let me prioritize the more popular libraries over the less popular
ones.

If you have any questions, comments and other support questions about how to
use this library, please use the
[GitHub Discussions](https://github.com/bxparks/EpoxyDuino/discussions)
for this project. If you have bug reports or feature requests, please file a
ticket in [GitHub Issues](https://github.com/bxparks/EpoxyDuino/issues).
I'd love to hear about how this software and its documentation can be improved.
I can't promise that I will incorporate everything, but I will give your ideas
serious consideration.

Please refrain from emailing me directly unless the content is sensitive. The
problem with email is that I cannot reference the email conversation when other
people ask similar questions later.

<a name="Authors"></a>
## Authors

* Created by Brian T. Park (brian@xparks.net).
* Support for using as library, by making `main()` a weak reference, added
  by Max Prokhorov (prokhorov.max@outlook.com).
* Add `delayMicroSeconds()`, `WCharacter.h`, and stub implementations of
  `IPAddress.h`, `SPI.h`, by Erik Tideman (@ramboerik), see
  [PR #18](https://github.com/bxparks/EpoxyDuino/pull/18).
* Add `memcpy_P()` and `vsnprintf_P()` by @pmp-p, 
  [PR #28](https://github.com/bxparks/EpoxyDuino/pull/28).
