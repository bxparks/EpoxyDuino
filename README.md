# EpoxyDuino

[![AUnit Tests](https://github.com/bxparks/EpoxyDuino/actions/workflows/aunit_tests.yml/badge.svg)](https://github.com/bxparks/EpoxyDuino/actions/workflows/aunit_tests.yml)

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

The `Serial` port object sends the output to the `STDOUT` and reads from the
`STDIN` of the Unix host environment. Most other hardware dependent
features (e.g. I2C, SPI, GPIO) are stubbed out (defined but don't do anything)
to allow the Arduino programs to compile. Mock versions of various libraries are
also provided:

* `<Wire.h>`: mock I2C library
* `<SPI.h>`: mock SPI library
* [EpoxyMockDigitalWriteFast](libraries/EpoxyMockDigitalWriteFast): mock
  version of the `digitalWriteFast` libraries
* [EpoxyMockTimerOne](libraries/EpoxyMockTimerOne): mock version of the
  TimerOne (https://github.com/PaulStoffregen/TimerOne) library
* [EpoxyMockFastLED](libraries/EpoxyMockFastLED/): mock version of the
  FastLED (https://github.com/FastLED/FastLED) library
* [EpoxyMockSTM32RTC](libraries/EpoxyMockSTM32RTC/): mock version of the
  STM32RTC (https://github.com/stm32duino/STM32RTC) library

These mock libraries may be sufficient for a CI pipeline.

For actual application development, I have started to build a set of
libraries within EpoxyDuino which emulate the versions that run the actual
hardware:

* [EpoxyFS](libraries/EpoxyFS): emulation of the ESP8266 LittleFS or
  ESP32 LittleFS filesystem
* [EpoxyEepromAvr](libraries/EpoxyEepromAvr): emulation of AVR-flavored `EEPROM`
* [EpoxyEepromEsp](libraries/EpoxyEepromEsp): emulation of ESP-flavored `EEPROM`

If your program has limited hardware dependencies so that it is conceptually
portable to a vanilla Unix environment, EpoxyDuino may work well for you.

Running an Arduino program natively on a desktop-class machine has some
advantages:

* The development cycle can be lot faster because the compilers on the
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

**Version**: 1.5.0 (2022-12-08)

**Changelog**: See [CHANGELOG.md](CHANGELOG.md)

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
    * [Additional Cpp Flags](#AdditionalCppFlags)
    * [Additional Compiler Flags](#AdditionalCompilerFlags)
    * [Additional Clean Up](#AdditionalCleanUp)
    * [Additional Dependencies](#AdditionalDependencies)
    * [Generated Source Code](#GeneratedSourceCode)
    * [Alternate Arduino Core](#AlternateArduinoCore)
    * [PlatformIO](#PlatformIO)
    * [Command Line Flags and Arguments](#CommandLineFlagsAndArguments)
    * [Debugging](#Debugging)
        * [Valgrind](#Valgrind)
    * [Mock digitalRead() digitalWrite()](#MockDigitalReadDigitalWrite)
        * [digitalReadValue()](#DigitalReadValue)
        * [digitalWriteValue()](#DigitalWriteValue)
* [Supported Arduino Features](#SupportedArduinoFeatures)
    * [Arduino Functions](#ArduinoFunctions)
    * [Serial Port Emulation](#SerialPortEmulation)
        * [Unix Line Mode](#UnixLineMode)
        * [Enable Terminal Echo](#EnableTerminalEcho)
* [Libraries and Mocks](#LibrariesAndMocks)
    * [Inherently Compatible Libraries](#InherentlyCompatibleLibraries)
    * [Emulation Libraries](#EmulationLibraries)
    * [Mock Libraries](#MockLibraries)
* [System Requirements](#SystemRequirements)
* [License](#License)
* [Bugs And Limitations](#BugsAndLimitations)
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
`{sketchbook_directory}/libraries/EpoxyDuino`, and put you on the default
`develop` branch.

You can be slightly conservative and use the latest stable release on the
`master` branch:
```
$ cd {sketchbook_directory}/libraries/EpoxyDuino
$ git checkout master
```

You can go to a specific release by checking out the corresponding tag, for
example `v1.2.0`:
```
$ git checkout v1.2.0
```

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

The executable will be created with a `.out` extension. To run it, type
one of the following:
```
$ ./BlinkSOS.out
$ make run
```

If you run `:make run` command inside the `vim` editor, it knows how to parse
the file and line number contained in the assertion messages generated by
[AUnit](https://github.com/bxparks/AUnit). The `vim` editor will jump directly
to the file and line where the assertion failure occurred. See
https://vimhelp.org/quickfix.txt.html for information on the `quickfix` feature
and the `errorformat` format that parses the GNU Make output.

The output that would normally be printed on the `Serial` on an Arduino
board will be sent to the `STDOUT` of the Linux, MacOS, or FreeBSD terminal. The
output should be identical to what would be shown on the serial port of the
Arduino controller.

<a name="AdditionalLibraries"></a>
### Additional Arduino Libraries

If the Arduino program depends on additional Arduino libraries, they must be
specified in the `Makefile` using the `ARDUINO_LIBS` parameter. For example,
the following includes the [AUnit](https://github.com/bxparks/AUnit),
[AceButton](https://github.com/bxparks/AceButton), and
[AceTime](https://github.com/bxparks/AceTime) libraries if they are installed at
the same directory level as EpoxyDuino:

```
APP_NAME := SampleTest
ARDUINO_LIBS := AUnit AceButton AceTime
include ../../EpoxyDuino/EpoxyDuino.mk
```

The libraries are referred to using their base directory name (e.g. `AceButton`,
or `AceTime`) not their full path. By default, the `EpoxyDuino.mk` file will look
for these additional libraries at the following locations:

* `EPOXY_DUINO_DIR/../` - in other words, siblings to the `EpoxyDuino` install
  directory (this assumes that EpoxyDuino was installed in the Arduino
  `libraries` directory as recommended above)
* `EPOXY_DUINO_DIR/libraries/` - additional libraries provided by the EpoxyDuino
  project itself
* under each of the additional directories listed in `ARDUINO_LIB_DIRS` (see
  below)

Version v1.5 supports compiling and linking plain C files in the third-party
libraries (defined by `ARDUINO_LIBS`) or in the application directory itself.
The C files are assumed to be written in C11 and are compiled using the `cc`
compiler. It may be possible to override the compiler flags by adding `-std=c99`
into the `EXTRA_CFLAGS` in the Makefile (which clobbers the default `-std=c11`)
but this has not been tested.

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
  believe it simply concatenates them all into a single file.) EpoxyDuino
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
|   `-- foolib
|       |-- file.h
|       `-- file.cpp
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
		$(MAKE) -C $$(dirname $$i); \
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
		$(MAKE) -C $$(dirname $$i); \
	done

runtests:
	set -e; \
	for i in *Test/Makefile; do \
		echo '==== Running:' $$(dirname $$i); \
		$(MAKE) -C $$(dirname $$i) run; \
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

If you run `:make runtests` inside the `vim` editor, it knows how to parse the
diagnostic outputs generated by GNU Make (as it changes directories through the
`-C` flag), and it knows how to parse the file and line number contained in the
assertion messages generated by [AUnit](https://github.com/bxparks/AUnit). The
`vim` editor will jump directly to the file and line where the assertion failure
occurred. See https://vimhelp.org/quickfix.txt.html for information on the
`quickfix` feature and the `errorformat` format that parses the GNU Make output.

These parent Makefiles can also be used in Continuous Integration, as shown
below.

<a name="ContinuousIntegration"></a>
### Continuous Integration

You can use EpoxyDuino to run continuous integration tests or
validations on the [GitHub Actions](https://github.com/features/actions)
infrastructure. The basic `ubuntu-18.04` or `ubuntu-20.04` docker image already
contains the C++ compiler and `make` binary. You don't need to install the
Arduino IDE or the Arduino CLI. You need:

* EpoxyDuino,
* your project that you want to test,
* any additional Arduino libraries that you use.

Take a look at some of my GitHub Actions YAML config files:

* [.github/workflows](.github/workflows) used by this project
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
you can use that instead by specifying the `CXX` makefile variable:
```
$ make CXX=clang++
```

This tells `make` to set the `CXX` variable to `clang++` within the context of
`EpoxyDuino.mk` which causes `clang++` to be used over the default `g++`.

If you have C files in your library or application, you can use to following to
compile the C files using `clang` instead of `cc` (which is the same as `gcc` on
Linux):
```
$ make CXX=clang++ CC=clang
```

One reason to use `clang++` instead of `g++` is that the `clang++` compiler will
sometimes catch a different set of programming errors.

<a name="AdditionalCppFlags"></a>
### Additional Cpp Flags

You can pass additional flags to the C preprocessor through the `EXTRA_CPPFLAGS`
variable, like this:

```
$ make EXTRA_CPPFLAGS='-D DEBUG=2'
```

<a name="AdditionalCompilerFlags"></a>
### Additional Compiler Flags

You can pass additional flags to the C++ compiler through the `EXTRA_CXXFLAGS`
variable, like this:

```
$ make EXTRA_CXXFLAGS='-g'
```

or

```
$ make EXTRA_CXXFLAGS='-g' EXTRA_CFLAGS='-g'
```

<a name="AdditionalCleanUp"></a>
### Additional Clean Up

The `make clean` rule is predefined to remove all of the intermediate `*.o`
files and `GENERATED` files that the `EpoxyDuino.mk` file knows about.
Sometimes, we want to do additional clean up. For example, the EEPROM emulation
libraries ([EpoxyEepromAvr](libraries/EpoxyEepromAvr) or
[EpoxyEepromEsp](libraries/EpoxyEepromEsp)) will create a file in the current
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

<a name="AdditionalDependencies"></a>
### Additional Dependencies

Sometimes the `*.ino` file depends on additional header files within the same
directory. When these header files are modified, the `*.ino` file must be
recompiled. These additional header files can be listed in the `DEPS` variable:

```
DEPS := header1.h header2.h
...
include {path/to/EpoxyDuino.mk}
```

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

The `GENERATED` is not strictly required, since the default rules already know
how to compile the `*.o` files from the `*.cpp` or `*.c` files. The primary
effect of `GENERATED` currently is to cause the generated files to be removed
when `make clean` is called.

<a name="AlternateArduinoCore"></a>
### Alternate Arduino Core

This is very advanced. The Arduino ecosystem supports different hardware
processors, architectures, and platforms. The software environment for a
specific hardware environment is called a "Core". By default, the environment
provided by EpoxyDuino resembles the AVR Core most closely because a lot of the
API emulation code was borrowed from the AVR Core.

There may be situations where an Arduino program is specifically meant to run
under a hardware platform other than an AVR processor, for example, the ESP8266
Core. EpoxyDuino provides the ability substitute a different Arduino API Core
through 2 Makefile variables:

* `EPOXY_CORE`
* `EPOXY_CORE_PATH`

#### `EPOXY_CORE`

The `EPOXY_CORE` Makefile variable defines the C-preprocessor macro which will
be defined through the `-D` flag through `-D $(EPOXY_CORE)`.

There are currently 2 valid options for this Makefile variable:

* `EPOXY_CORE_AVR` (default)
    * Causes `Arduino.h` to emulate the Arduino AVR Core.
* `EPOXY_CORE_ESP8266`
    * Causes `Arduino.h` to emulate the ESP8266 Core.

For example, setting the following in the `Makefile`:

```
EPOXY_CORE := EPOXY_CORE_ESP8266
```

causes the `make` command to pass the `-D EPOXY_CORE_ESP8266` flag to the
compiler, which will activate any code that is guarded by:

```C++
#if defined(EPOXY_CORE_ESP8266)
  ...
#endif
```

Note that:

* `EPOXY_CORE_AVR` does **not** define the `ARDUINO_ARCH_AVR` macro,
* `EPOXY_CORE_ESP8266` does **not** define the `ESP8266` or the
  `ARDUINO_ARCH_ESP8266` macros.

This is because EpoxyDuino cannot provide perfect emulation of all the classes
and APIs of the AVR, the ESP8266, or any other third party cores. So defining
these macros would break too much code. The developer must carefully evaluate
whether a `#if defined(EXPOXY_CORE_ESP8266)` should be enabled for code that is
guarded by `#if defined(ESP8266)`.

#### `EPOXY_CORE_PATH`

If the `EPOXY_CORE` make variable is insufficient (e.g. because the appropriate
changes have not been incorporated into `$(EPOXY_DUINO_DIR)/cores/epoxy/`), then
the `EPOXY_CORE_PATH` provides an even bigger hammer. It defines the full-path
to the Arduino Core API files.

By default, this is set to `$(EPOXY_DUINO_DIR)/cores/epoxy`. You can create your
own set of Arduino API files in a directory of your choosing, and set this
make variable to point to these custom files:

```
EPOXY_CORE_PATH := {my_own_directory}/cores/mycore
```

<a name="PlatformIO"></a>
### PlatformIO

The `library.json` file supports [PlaformIO in Native
mode](https://docs.platformio.org/en/latest/platforms/native.html). It was added
in [PR#31](https://github.com/bxparks/EpoxyDuino/pull/31) (thanks
[@lopsided98](https://github.com/lopsided98)). However, this functionality is
*unsupported*. If it becomes broken in the future, please send me a PR to fix
it.

<a name="CommandLineFlagsAndArguments"></a>
### Command Line Flags and Arguments

The standard Arduino environment does not provide command line arguments, since
a microcontroller does not normally provide a command line environment.
When an Arduino application is compiled Using EpoxyDuino, the Unix command line
parameters (`argc` and `argv`) become available through 2 global variables:

* `extern int epoxy_argc`
* `extern const char* const* epoxy_argv`

The [examples/CommandLine](examples/CommandLine) program contains a basic
command line parser which can be copied and customized for different
applications:

```
$ ./CommandLine.out --help
Usage: ./CommandLine.out [--help|-h] [-s] [--include word] [--] [words ...]

$ ./CommandLine.out one two
arg: one
arg: two

$ ./CommandLine.out -s
flag: -s

$ ./CommandLine.out --include inc one two
flag: --include inc
arg: one
arg: two

$ ./CommandLine.out --include inc -- -one two
flag: --include inc
arg: -one
arg: two

$ ./CommandLine.out -a
Unknonwn flag '-a'
Usage: ./CommandLine.out [--help|-h] [-s] [--include word] [--] [words ...]
```

A more advanced example can be seen in
[AUnit/TestRunner.cpp](https://github.com/bxparks/AUnit/blob/develop/src/aunit/TestRunner.cpp).

<a name="Debugging"></a>
### Debugging

A huge benefit of compiling Arduino programs using EpoxyDuino is that all the
debugging tools in a Unix environment become automatically available. For
example:

* External Tools
    * [Valgrind](https://valgrind.org/docs/manual/quick-start.html)
    * [GDB Debugger](https://www.sourceware.org/gdb/)
* Compiler Options
    * [Address Sanitizer](https://github.com/google/sanitizers/wiki/AddressSanitizer) (ASan)
    * [Memory Sanitizer](https://github.com/google/sanitizers/wiki/MemorySanitizer)
      (MSan)
    * [Undefined Behavior Sanitizer](https://clang.llvm.org/docs/UndefinedBehaviorSanitizer.html) (UBSan)

I am not an expert on any of these sanitizers, and I have not enabled them by
default in the `EpoxyDuino.mk` file. But you have the capability to add them to
your `Makefile` through the `EXTRA_CXXFLAGS` variable.

Below are some things that I have found useful in my own limited experience.

<a name="Valgrind"></a>
#### Valgrind

I have found the [Valgrind](https://valgrind.org/docs/manual/quick-start.html)
tool quite helpful in tracking down Segmentation Fault crashes. Here is a quick
start:

1. Compile your program using the `-g` flag.
    * This is not strictly necessary but this will allow Valgrind to print line
      numbers to the source code in the stack trace.
    * Two ways:
        * Pass the pass through the command line: `$ make EXTRA_CXXFLAGS=-g`
        * Edit the `Makefile` and add a `EXTRA_CXXFLAGS = -g` directive
          near the top of the file.
2. Run the program under the `valgrind` program.
    * Valgrind has tons of options and flags. Here are the flags that I use
      (don't remember where I got them):
        * `$ alias val='valgrind --tool=memcheck --leak-check=yes
        --show-reachable=yes --num-callers=20 --track-fds=yes'`
        * `$ val ./MyProgram.out`

When the program crashes because of a `nullptr` dereference, Valgrind will show
exactly where that happened in the source code.

<a name="MockDigitalReadDigitalWrite"></a>
### Mock digitalRead() digitalWrite()

EpoxyDuino is not meant to simulate the actual hardware. By default, the
`digitalRead()` and `digitalWrite()` functions are just stubs which don't do
anything. However for testing purposes, it is sometimes useful to be able to
control the values returned by `digitalRead()`, or to read back the value
written by `digitalWrite()`. Two functions have been added to EpoxyDuino to
allow this mocking.

* `void digitalReadValue(uint8_t pin, uint8_t val)`
    * Sets the value returned by the subsequent `digitalRead(pin)` to `val`.
* `uint8_t digitalWriteValue(uint8_t pin)`
    * Returns the value of the most recent `digitalWrite(pin, val)`.

<a name="DigitalReadValue"></a>
#### digitalReadValue()

The `digitalReadValue(pin, val)` function sets the value that will be
returned by the next `digitalRead(pin)`. Here is an example of how this can be
used:

```C++
#include <Arduino.h>

...
const uint8_t PIN = 8;

void something() {
  uint8_t val = digitalRead(PIN); // val == 0

#if defined(EPOXY_DUINO)
  digitalReadValue(PIN, 1);
#endif
  val = digitalRead(PIN); // val == 1

#if defined(EPOXY_DUINO)
  digitalReadValue(PIN, 0);
#endif
  val = digitalRead(PIN); // val == 0
}
```

The `#if defined(EPOXY_DUINO)` is recommended because `digitalReadValue()` is
not a standard Arduino function. It is defined only in EpoxyDuino.

The `pin` parameter should satisfy `0 <= pin < 32`. If `pin >= 32`, then
`digitalReadValue()` is a no-op and the corresponding `digitalRead(pin)` will
always return 0.

<a name="DigitalWriteValue"></a>
#### digitalWriteValue()

The `digitalWriteValue(pin)` function returns the value that was written by
the most recent `digitalWrite(pin, val)`. Here is an example of how this can be
used:

```C++
#include <Arduino.h>

...
const uint8_t PIN = 9;

void something() {
  digitalWrite(PIN, 0);

#if defined(EPOXY_DUINO)
  uint8_t val = digitalWriteValue(PIN);
  // val should be 0
#endif

  digitalWrite(PIN, 1);

#if defined(EPOXY_DUINO)
  uint8_t val = digitalWriteValue(PIN);
  // val should be 1
#endif
}
```

The `#if defined(EPOXY_DUINO)` is recommended because `digitalWriteValue()` is
not a standard Arduino function. It is defined only in EpoxyDuino.

The `pin` parameter should satisfy `0 <= pin < 32`. If `pin >= 32`, then
`digitalWriteValue()` always return 0.

<a name="SupportedArduinoFeatures"></a>
## Supported Arduino Features

<a name="ArduinoMacros"></a>
### Arduino Macros

The following C-preprocessor macros are defined:

* `ARDUINO=100`
    * Indicates compilation under Arduino.
* `EPOXY_DUINO`
    * Indicates compilation under EpoxyDuino
* `UNIX_HOST_DUINO`
    * For backwards compatibility with previous version of `EpoxyDuino`
      which was named `UnixHostDuino`.
* `EPOXY_CORE_AVR`
    * Defined when the Makefile variable `EPOXY_CORE` is set to
      `ARDUINO_ARCH_AVR`.
    * This is the default.
* `EPOXY_CORE_ESP8266`
    * Defined when the Makefile variable `EPOXY_CORE` is set to
      `ARDUINO_ARCH_ESP8266`.

The following are **not** defined. Many third party libraries tend to break with
those defined because EpoxyDuino does not emulate those environments perfectly:

* `ARDUINO_ARCH_AVR`
* `ARDUINO_ARCH_ESP8266`
* `ARDUINO_ARCH_ESP32`
* `ESP8266`
* `ESP32`

<a name="ArduinoFunctions"></a>
### Arduino Functions

The following is an incomplete list of Arduino functions and features
which are implemented:

* `Arduino.h`
    * `setup()`, `loop()`
    * `delay()`, `yield()`, `delayMicroSeconds()`
    * `millis()`, `micros()`
    * `digitalWrite()`, `digitalRead()`, `pinMode()` (empty stubs)
    * `analogRead()`, `analogWrite()` (empty stubs)
    * `pulseIn()`, `pulseInLong()`, `shiftIn()`, `shiftOut()` (empty stubs)
    * `min()`, `max()`, `abs()`, `round()`, etc
    * `bit()`, `bitRead()`, `bitSet()`, `bitClear()`, `bitWrite()`
    * `random()`, `randomSeed()`, `map()`
    * `makeWord()`
    * `F_CPU`, `clockCyclesPerMicrosecond()`, `clockCyclesToMicroseconds()`,
      `microsecondsToClockCycles()`
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
      `strncmp_P()`, `strcasecmp_P()`, `strchr_P()`, `strrchr_P()`, `strstr_P()`
    * `memcpy_P()`, `memcmp_P()`, `vsnprintf_P()`
    * `PROGMEM`, `PGM_P`, `PGM_VOID_P`, `PSTR()`
* `IPAddress.h`
    * `IPAddress` class
* `WCharacter.h`
    * `isAlpha()`, `isAscii()`, etc.
    * `toLowerCase()`, `toUpperCase()`, etc
* `Wire.h` (stub implementation)
* `SPI.h` (stub implementation)

See [Arduino.h](cores/epoxy/Arduino.h)
for the latest list. Most of the header files included by this `Arduino.h`
file were copied and modified from the [arduino:avr
core](https://github.com/arduino/ArduinoCore-avr/tree/master/cores/arduino),
v1.8.2 or v1.8.3. A number of tweaks have been made to support slight variations
in the API of other platforms, particularly the ESP8266 v2.7.4 and ESP32 v1.0.6
cores.

The `Print::printf()` function is an extension to the `Print` class that is
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

<a name="UnixLineMode"></a>
#### Unix Line Mode

The `Print` class in the Arduino API implements the `Print::println()` function
by printing the DOS line terminator characters `\r\n`. This decision make sense
when the serial port of the microcontroller is connected to a serial terminal,
which requires a `\r\n` at the end of each line to render the text properly.

But when the Arduino application is executed on Linux machine, and the output is
redirected into a file, the `\r\n` is not consistent with the Unix convention
of using only a single `\n` to terminate each line. This causes the file to be
interpreted as a DOS-formatted file. Usually the DOS formatted file can be
processed without problems by other Linux programs and scripts, but sometimes
the extra `\r\n` causes problems, especially when mixed with a `Serial.printf()`
function using a single `\n`.

EpoxyDuino provides a mechanism to configure the line termination convention for
a given application by providing 2 additional methods to its `Print` class:

```C++
class Print {
  public:
    // Use DOS line termination. This is the default.
    void setLineModeNormal();

    // Use Unix line termination.
    void setLineModeUnix();

    ...
};
```

When an Arduino application is executed on a Linux machine using EpoxyDuino,
you can configure the `Serial` object in the `*.ino` file to use the Unix
convention like this:

```C++
void setup() {
#if ! defined(EPOXY_DUINO)
  delay(1000); // wait to prevent garbage on Serial
#endif

  Serial.begin(115200);
  while (!Serial); // Leonardo/Micro

#if defined(EPOXY_DUINO)
  Serial.setLineModeUnix();
#endif
}
```

Why isn't `setLineModeUnix()` simply made to be the default on EpoxyDuino?
Because people write [AUnit](https://github.com/bxparks/AUnit) unit tests which
they expect will pass on both the microcontroller and on EpoxyDuino:

```C++
#include <Arduino.h>
#include <AUnit.h>
#include <AceCommon.h> // PrintStr<N>
...

static void sayHello(Print& printer) {
  printer.println("hello");
}

test(myTest) {
  PrintStr<200> observed;
  sayHello(observed);
  assertEqual(observed.cstr(), "hello\r\n");
}
```

<a name="EnableTerminalEcho"></a>
#### Enable Terminal Echno

By default, the `stdin` of the terminal is set to `NOECHO` mode for consistency
with the actual serial port of an Arduino microcontroller. However when running
a command line utility on a Unix terminal emulator using EpoxyDuino, it is often
useful to enable echoing so that the characters being typed are visible.

To enable echoing, call the `enableTerminalEcho()` function from the global
`setup()`:

```C++
void setup() {
  ...

#if defined(EPOXY_DUINO)
  enableTerminalEcho();
#endif

  ...
}
```

<a name="LibrariesAndMocks"></a>
## Libraries and Mocks

The Arduino ecosystem provides thousands of libraries that can be used to extend
the functionality of an Arduino application. Some of these libraries will
work perfectly fine with EpoxyDuino, some will not. It is difficult to
categorize these libraries in a sensible way in the context of EpoxyDuino, but
here is my current attempt:

* **Inherently Compatible Libraries**:
    * Libraries that are mostly algorithmic often have limited dependency on
      low-level Arduino API (e.g. `millis()`, `micros()`, `delay()`, `F()`).
    * If these have been written to be cross-platform across different Arduino
      hardware, then these should also automatically work under EpoxyDuino
      with little or no modifications.
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
because EpoxyDuino is what I use to develop and test my libraries. For example,
the following should compile using EpoxyDuino:

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
EpoxyDuino. I have provided 3 such libraries within the EpoxyDuino project:

* [libraries/EpoxyFS](libraries/EpoxyFS)
    * An implementation of a file system compatible with
      [ESP8266 LittleFS](https://arduino-esp8266.readthedocs.io/en/latest/filesystem.html)
      and [ESP32 LitteFS](https://github.com/espressif/arduino-esp32/tree/master/libraries/LittleFS).
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
intended. This limitation may be sufficient for Continuous Integration purposes.

* Wire
    * The `<Wire.h>` header file is provided automatically by the `<Arduino.h>`
      file in EpoxyDuino. No additional library needs to be added to the
      `ARDUINO_LIBS` variable in the `Makefile`.
    * It provides only mock functions of the actually `Wire` library that
      is provided by real Arduino frameworks.
    * This was added very early in the development of EpoxyDuino so that I could
      compile some of my programs. I don't think I realized at the time that
      `Wire` is a separate (but built-in) library. In retrospect, it may have
      been better to split this file into a separate mock library.
* SPI
    * The `<SPI.h>` header file was contributed recently (see #18 and #19) and
      is included automatically by the `<Arduino.h>` file in EpoxyDuino.
    * It follows the same pattern as `Wire`, the header file provides only mock
      functions of the actual `SPI` library.
* [EpoxyMockDigitalWriteFast](libraries/EpoxyMockDigitalWriteFast)
    * A simple mock of one of the `digitalWriteFast` libraries (e.g.
      https://github.com/NicksonYap/digitalWriteFast) to allow code written
      against it to compile under EpoxyDuino.
* [EpoxyMockTimerOne](libraries/EpoxyMockTimerOne)
    * A simple mock of the TimerOne (https://github.com/PaulStoffregen/TimerOne)
      library.
* [EpoxyMockFastLED](libraries/EpoxyMockFastLED/)
    * Mock version of the FastLED (https://github.com/FastLED/FastLED) library.
* [EpoxyMockSTM32RTC](libraries/EpoxyMockSTM32RTC/)
    * Mock version of the STM32RTC (https://github.com/stm32duino/STM32RTC)
      library.
* EspMock (https://github.com/hsaturn/EspMock)
    * This is a separate project that provides various mocks for functions and
      libraries included with the ESP8266 and the ESP32 processors.
    * It is not an Arduino library, so it needs to be installed using a manual
      `git clone`.

<a name="SystemRequirements"></a>
## System Requirements

**Tier 1: Fully Supported**

The following environments are tested on each release of EpoxyDuino.

* Ubuntu 20.04.4 LTS
    * g++ (Ubuntu 9.4.0-1ubuntu1~20.04.1) 9.4.0
    * clang++ version 10.0.0-4ubuntu1
    * GNU Make 4.2.1

**Tier 2: Best Effort**

The following environments are supported on a best-effort basis because I don't
test them as often.

* MacOS 11.6.7 (Big Sur)
    * clang++
        * Apple clang version 13.0.0 (clang-1300.0.29.30)
        * Target: x86_64-apple-darwin20.6.0
    * GNU Make 3.81
    * (Big Sur is the latest MacOS that I am able to test.)
* Raspbian GNU/Linux 10 (buster)
    * On Raspberry Pi Model 3B
    * g++ (Raspbian 8.3.0-6+rpi1) 8.3.0
    * GNU Make 4.2.1
* FreeBSD 12.2
    * c++: FreeBSD clang version 10.0.1
    * gmake: GNU Make 4.3
        * Install using `$ pkg install gmake`
        * You can type `gmake` instead of `make`, or
        * Create a shell alias, or
        * Create a symlink in `~/bin`.

**Tier 3: Should Work**

The following environments are older OS environments which worked with previous
versions of EpoxyDuino. But I am not able to validate them against the latest
EpoxyDuino release because I no longer use these older environments.

* Ubuntu 18.04 LTS
    * g++ (Ubuntu 7.5.0-3ubuntu1~18.04) 7.5.0
    * clang++ 8.0.0-3~ubuntu18.04.2
    * clang++ 6.0.0-1ubuntu2
    * GNU Make 4.1
* MacOS 10.14.6 (Mojave)
    * Apple clang version 11.0.0 (clang-1100.0.33.17)
    * GNU Make 3.81
* MacOS 10.14.5 (Mojave)
    * clang++ Apple LLVM version 10.0.1
    * GNU Make 3.81

<a name="License"></a>
## License

[MIT License](https://opensource.org/licenses/MIT)

<a name="BugsAndLimitations"></a>
## Bugs and Limitations

* There is no formal specification of the "Arduino API" that I am aware of.
    * The reference documentation at https://www.arduino.cc/reference/ may be
      good enough for beginners to blink a few LED lights, but it is not
      sufficient to build an API emulator on a Linux machine.
    * The version of the Arduino API implemented in this library has been
      reverse engineered and inferred from
      [ArduinoCore-avr](https://github.com/arduino/ArduinoCore-avr), either
      v1.8.2 and v1.8.3 (I cannot remember).
    * Each third party Arduino-compatible platform (e.g. STM32, ESP8266, ESP32)
      has implemented a slightly different version of the "Arduino API".
    * EpoxyDuino does not support the idiosyncrasies of all of these
      different Arduino platforms.
    * A few features relevant to the ESP8266 and ESP32 platforms can
      be activated by setting `EPOXY_CORE := EPOXY_CORE_ESP8266` in the
      `Makefile`. See [Alternate Arduino Core](#AlternateArduinoCore).
* There is yet another version of the "Arduino API" described by
  [ArduinoCore-API](https://github.com/arduino/ArduinoCore-API).
    * Some Arduino-branded microcontrollers have been migrated to
      this new API (e.g. Nano Every, MKR1000, Nano 33 IoT).
    * The new Arduino API has a number of backwards incompatible changes to
      the old Arduino API.
    * EpoxyDuino does *not* support this new Arduino API.
* The Arduino API on a microcontroller automatically provides a `main()`
  function that calls the global `setup()` function, then calls the global
  `loop()` function forever, as fast as possible.
    * The EpoxyDuino version of `main()` calls `loop()` with a delay of 1 ms
      per iteration. Without the 1 ms delay, the application consumes 100% of
      CPU time on the host computer.
    * This means that the `loop()` function is called at a maximum frequency of
      1000 Hz.
* The Serial port emulation provided by `StdioSerial` may be buggy or behave in
  non-intuitive ways.
    * When the application is executed without input or output redirection, the
      *stdin* is put into "raw" mode.
    * If either the input or output is redirected (e.g. output redirected to a
      file), then the *stdin* remains in normal Unix "cooked" mode.
    * This allows the Arduino program to be piped into a screen pager (e.g.
      `less(1)`, while allowing the `less(1)` program to support its normal
      keyboard control keys.
    * The *stdout* is wired directly into the POSIX `write()` function, which is
      unbuffered. This may cause performance problems when generating a lot of
      output.
* The compiler used to compile the microcontroller binary may be significantly
  different than the compiler used on the host Unix computer, even if they are
  both `g++`.
    * I am not sure that I have migrated all the relevant and important compiler
      flags from the microcontroller environment (AVR, ESP8266, etc.) to
      the EpoxyDuino environment.

<a name="FeedbackAndSupport"></a>
## Feedback and Support

If you have any questions, comments, or feature requests for this library,
please use the [GitHub
Discussions](https://github.com/bxparks/EpoxyDuino/discussions) for this
project. If you have bug reports, please file a ticket in [GitHub
Issues](https://github.com/bxparks/EpoxyDuino/issues). Feature requests should
go into Discussions first because they often have alternative solutions which
are useful to remain visible, instead of disappearing from the default view of
the Issue tracker after the ticket is closed.

Please refrain from emailing me directly unless the content is sensitive. The
problem with email is that I cannot reference the email conversation when other
people ask similar questions later.

<a name="Authors"></a>
## Authors

* Created by Brian T. Park (brian@xparks.net).
* Support for using as library, by making `main()` a weak reference,
  by Max Prokhorov (@mcspr), see
  [PR#6](https://github.com/bxparks/EpoxyDuino/pull/6).
* Add `delayMicroSeconds()`, `WCharacter.h`, and stub implementations of
  `IPAddress.h`, `SPI.h`, by Erik Tideman (@ramboerik), see
  [PR#18](https://github.com/bxparks/EpoxyDuino/pull/18).
* Add `memcpy_P()` and `vsnprintf_P()` by Paul m. p. P. (@pmp-p),
  [PR#28](https://github.com/bxparks/EpoxyDuino/pull/28).
* Support PlatformIO native mode, by Ben Wolsieffer (@lopsided98),
  see [PR#31](https://github.com/bxparks/EpoxyDuino/pull/31).
* Move stdin processing to `yield()` by Ben Wolsieffer (@lopsided98),
  see [PR#32](https://github.com/bxparks/EpoxyDuino/pull/32).
* Simplify `StdioSerial` by Bernhard (@felias-fogg),
  [Issue#43](https://github.com/bxparks/EpoxyDuino/issues/43).
* Add `digitalReadValue(pin, val)` to control the return value of
  `digitalRead(pin)` by @CaioPellicani. See
  [PR#61](https://github.com/bxparks/EpoxyDuino/pull/61).
* Add `tone()` and `noTone()` stubs, by @kwisii in
  [PR#69](https://github.com/bxparks/EpoxyDuino/pull/69).
* Add `uint8_t digitalWriteValue(pin)` by @kwisii in
  [PR#68](https://github.com/bxparks/EpoxyDuino/pull/68).
* Add `memcmp_P()` by @dawidchyrzynski in
  [PR#71](https://github.com/bxparks/EpoxyDuino/pull/71).
