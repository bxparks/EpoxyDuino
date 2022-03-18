// This header file is empty. Its sole purpose is to disable warnings in the
// Arduino IDE which identifies EpoxyDuino as an invalid Arduino library when it
// is installed inside the Arduino sketchbook libraries directory. The warning
// message looks like this:
//
// "Invalid library found in /home/brian/src/arduino/libraries/EpoxyDuino: no
// headers files (.h) found in /home/brian/src/arduino/libraries/EpoxyDuino"
//
// EpoxyDuino is *not* an Arduino library. But it is very convenient to install
// it along side the other Arduino libraries so that it can search for those
// libraries as siblings to its own location when they are referenced in the
// `ARDUINO_LIBS` Makefile variable. It can be installed somewhere else, but you
// then have to define the `ARDUINO_LIB_DIRS` variable in all your Makefiles to
// tell EpoxyDuino where to look for those libraries.
