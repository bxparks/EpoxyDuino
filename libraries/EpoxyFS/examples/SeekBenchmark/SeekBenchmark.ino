/*
 * Copyright (c) 2022 Brian T. Park
 * Commercial License
 */

/*
Benchmark the following operations:

* write()
    * write 300 records of 100 random bytes, total 30kB
* read()
    * read 300 records of 100 bytes
    * no call to seek() before reading
* seek()/read()
    * call seek(offset) before reading a record of 100 bytes
    * iterate for 300 records
    * the offset is the *current* position so in theory the library code
      ought to be able to optimize that away, but does not
    * called twice to reduce effect of any caching

**EpoxyDuino**
```
BENCHMARKS
write() 231
seek()/read() 1279
seek()/read() 1129
read() 144
END
```

**ESP8266**
```
BENCHMARKS
write() 472975
seek()/read() 32162
seek()/read() 31921
read() 12737
END
```

**ESP32**
```
BENCHMARKS
write() 245635
seek()/read() 18123
seek()/read() 18065
read() 15041
END
```
*/

#include <Arduino.h>

#ifndef SERIAL_PORT_MONITOR
  #define SERIAL_PORT_MONITOR Serial
#endif

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

#define TEST_FILE_NAME "/testfile.out"

const uint16_t ITERATION = 300;
const uint16_t READ_SIZE = 100;

//-----------------------------------------------------------------------------

// Read file with File::seek();
void seekReadFile(const char* label, fs::File& file) {
  file.seek(0);
  char buffer[READ_SIZE];
  uint16_t offset = 0;

  unsigned long startMicros = micros();
  for (uint16_t i = 0; i < ITERATION; i++) {
    file.seek(offset);
    file.readBytes(buffer, sizeof(buffer));
    offset += READ_SIZE;
  }
  unsigned long elapsedMicros = micros() - startMicros;

  SERIAL_PORT_MONITOR.print(label);
  SERIAL_PORT_MONITOR.print(' ');
  SERIAL_PORT_MONITOR.println(elapsedMicros);
}

// Read file without File::seek();
void readFile(const char* label, fs::File& file) {
  file.seek(0);
  char buffer[READ_SIZE];

  unsigned long startMicros = micros();
  for (uint16_t i = 0; i < ITERATION; i++) {
    file.readBytes(buffer, sizeof(buffer));
  }
  unsigned long elapsedMicros = micros() - startMicros;

  SERIAL_PORT_MONITOR.print(label);
  SERIAL_PORT_MONITOR.print(' ');
  SERIAL_PORT_MONITOR.println(elapsedMicros);
}

void runBenchmarks(const char* fileName) {
  File file = FILE_SYSTEM.open(fileName, "r");
  if (!file) {
    SERIAL_PORT_MONITOR.printf("Could not open '%s' for read.\n", fileName);
    return;
  }

  seekReadFile("seek()/read()", file); // warm up any cache
  seekReadFile("seek()/read()", file);
  readFile("read()", file);

  file.close();
}

void createFile(const char* label, const char* fileName) {
  File file = FILE_SYSTEM.open(fileName, "w");
  if (!file) {
    SERIAL_PORT_MONITOR.printf("Could not open '%s' for write.\n", fileName);
    return;
  }

  unsigned long startMicros = micros();
  char buffer[READ_SIZE]; // no initialization, so full of random stuff
  for (uint16_t i = 0; i < ITERATION; i++) {
    file.write((const uint8_t*) buffer, sizeof(buffer));
  }
  unsigned long elapsedMicros = micros() - startMicros;

  SERIAL_PORT_MONITOR.print(label);
  SERIAL_PORT_MONITOR.print(' ');
  SERIAL_PORT_MONITOR.println(elapsedMicros);
  file.close();
}

//-----------------------------------------------------------------------------

void setup() {
#if ! defined(EPOXY_DUINO)
  delay(1000); // some boards reboot twice
#endif

  SERIAL_PORT_MONITOR.begin(115200);
  while (!SERIAL_PORT_MONITOR); // For Leonardo/Micro
#if defined(EPOXY_DUINO)
  SERIAL_PORT_MONITOR.setLineModeUnix();
#endif

  SERIAL_PORT_MONITOR.println(F("Initializing file system"));
  if (! FILE_SYSTEM.begin()) {
    SERIAL_PORT_MONITOR.println(F("Error initializing file system"));
    exit(1);
  }

  SERIAL_PORT_MONITOR.println(F("BENCHMARKS"));
  createFile("write()", TEST_FILE_NAME);
  runBenchmarks(TEST_FILE_NAME);
  SERIAL_PORT_MONITOR.println("END");

#if defined(EPOXY_DUINO)
  exit(0);
#endif
}

void loop() {
}
