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

#if defined(EPOXY_DUINO)

#include <stdio.h> // fprintf()
#include <string.h> // strcmp()
#include <Arduino.h> // epoxy_argc, epoxy_argv
#include "ProcessCommandLine.h"

/** Shift argument parameters to the left by one slot. */
static void shift(int& argc, const char* const*& argv) {
  argc--;
  argv++;
}

/** Return true if 2 strings are equal. */
static bool argEquals(const char* s, const char* t) {
  return strcmp(s, t) == 0;
}

/** Print usage and exit the program with the given status code. */
static void usageAndExit(int status) {
  fprintf(
    stderr,
    "Usage: %s [--help|-h] [-s] [--include word] [--] [words ...]\n",
    epoxy_argv[0]
  );
  exit(status);
}

/**
 * Parse command line flags.
 * Returns the index of the first argument after the flags.
 */
static int parseFlags(int argc, const char* const* argv) {
  int argc_original = argc;
  shift(argc, argv); // skip the name of the program at argv[0]
  while (argc > 0) {
    if (argEquals(argv[0], "--include")) {
      shift(argc, argv);
      if (argc == 0) usageAndExit(1);
      fprintf(stderr, "flag: --include %s\n", argv[0]);
    } else if (argEquals(argv[0], "-s")) {
      fprintf(stderr, "flag: -s\n");
    } else if (argEquals(argv[0], "--")) {
      shift(argc, argv);
      break;
    } else if (argEquals(argv[0], "--help")
        || argEquals(argv[0], "-h")) {
      usageAndExit(0);
      break;
    } else if (argv[0][0] == '-') {
      fprintf(stderr, "Unknonwn flag '%s'\n", argv[0]);
      usageAndExit(1);
    } else {
      break;
    }
    shift(argc, argv);
  }

  return argc_original - argc;
}

void processCommandLine() {
  int args = parseFlags(epoxy_argc, epoxy_argv);

  // Process any remaining *space*-separated arguments.
  for (int i = args; i < epoxy_argc; i++) {
    fprintf(stderr, "arg: %s\n", epoxy_argv[i]);
  }
}

#endif
