/*
MIT License

Copyright (c) 2021 Brian T. Park

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

/*
 * CommandLine: A demo of parsing command line flags and arguments using
 * EpoxyDuino on a Unix-like environment.
 *
 * This program should print the following:
 *
 * @verbatim
 * $ make
 *
 * $ ./CommandLine.out --help
 * Usage: ./CommandLine.out [--help|-h] [-s] [--include word] [--] [words ...]
 *
 * $ ./CommandLine.out one two
 * arg: one
 * arg: two
 *
 * $ ./CommandLine.out -s
 * flag: -s
 *
 * $ ./CommandLine.out --include inc one two
 * flag: --include inc
 * arg: one
 * arg: two
 *
 * $ ./CommandLine.out --include inc -- -one two
 * flag: --include inc
 * arg: -one
 * arg: two
 *
 * $ ./CommandLine.out -a
 * Unknonwn flag '-a'
 * Usage: ./CommandLine.out [--help|-h] [-s] [--include word] [--] [words ...]
 *
 * @endverbatim
 */

#include <Arduino.h>
#include "ProcessCommandLine.h" // processCommandLine()

void setup() {
#if ! defined(EPOXY_DUINO)
  delay(1000); // some boards reboot twice
#endif
  SERIAL_PORT_MONITOR.begin(115200);
  while (!SERIAL_PORT_MONITOR); // For Leonardo/Micro

#if defined(EPOXY_DUINO)
  processCommandLine();
  exit(0);
#endif
}

void loop() {}
