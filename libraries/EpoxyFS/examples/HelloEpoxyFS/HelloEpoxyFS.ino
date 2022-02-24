/*
Test writing and reading to the filesystem using EpoxyFS on EpoxyDuino, and
LittleFS on ESP8266 and ESP32. The expected output on various platforms is shown
below:

EpoxyDuino:

```
== Initializing EpoxyFS
== Formatting file system
== List '/' using fs::Dir
== Writing /testfile.txt
Created /testfile.txt
== List '/' using fs::Dir
Dir entry #1
Dir: fileName()=testfile.txt
isDirectory=false
fileSize()=0
File: name()=testfile.txt
fullName()=/testfile.txt
size()=31
== Reading /testfile.txt
name(): testfile.txt
fullName(): /testfile.txt
This is a test
42
42.00
2A
== Recursively remove '/' using nftw()
File: epoxyfsdata/testfile.txt
Post Dir: epoxyfsdata
== List '/' using fs::Dir
== Done
```

# ESP8266:

```
== Initializing LittleFS
== Formatting file system
== List '/' using fs::Dir
== Writing /testfile.txt
Created /testfile.txt
== List '/' using fs::Dir
Dir entry #1
Dir: fileName()=testfile.txt
isDirectory=false
fileSize()=31
File: name()=testfile.txt
fullName()=testfile.txt
size()=31
== Reading /testfile.txt
name(): testfile.txt
fullName(): /testfile.txt
This is a test
42
42.00
2A
== Recursively remove '/' not implemented
== List '/' using fs::Dir
Dir entry #1
Dir: fileName()=testfile.txt
isDirectory=false
fileSize()=31
File: name()=testfile.txt
fullName()=testfile.txt
size()=31
== Done
```

# ESP32:

```
== Initializing LittleFS
== Formatting file system
== List '/' not implemented on ESP32
== Writing /testfile.txt
Created /testfile.txt
== List '/' not implemented on ESP32
== Reading /testfile.txt
name(): testfile.txt
path()=/testfile.txt
This is a test
42
42.00
2A
== Recursively remove '/' not implemented
== List '/' not implemented on ESP32
== Done
```
*/

#include <stdio.h> // remove()
#include <Arduino.h>

// ESP32 does not define SERIAL_PORT_MONITOR
#ifndef SERIAL_PORT_MONITOR
  #define SERIAL_PORT_MONITOR Serial
#endif

#if defined(EPOXY_DUINO)
  #define FILE_SYSTEM_NAME "EpoxyFS"
  #include <ftw.h> // nftw()
  #include <EpoxyFS.h>
  #define FILE_SYSTEM fs::EpoxyFS
  using fs::File;
  using fs::Dir;
#elif defined(ESP8266)
  #define FILE_SYSTEM_NAME "LittleFS"
  #include <LittleFS.h>
  #define FILE_SYSTEM LittleFS
  using fs::File;
  using fs::Dir;
#elif defined(ESP32)
  #define FILE_SYSTEM_NAME "LittleFS"
  #include <LittleFS.h>
  #define FILE_SYSTEM LittleFS
  using fs::File;
  //using fs::Dir; // not implemented on ESP32
#else
  #error Unsupported platform
#endif

#define TEST_FILE_NAME "/testfile.txt"

#if defined(EPOXY_DUINO)

int removeFile(
    const char *fpath,
    const struct stat * /*sb*/,
    int typeflag,
    struct FTW *ftwbuf)
{
  if (typeflag == FTW_F) {
    printf("File: %s\n", fpath);
  } else if (typeflag == FTW_SL) {
    printf("Symlink: %s\n", fpath);
  } if (typeflag == FTW_DP) {
    printf("Post Dir: %s\n", fpath);
  }
  int status = 0;
  if (ftwbuf->level != 0) {
    status = remove(fpath);
  }
  return status;
}

// Maximum number of open file descriptors.
static const int MAX_NOPEN_FD = 5;

// Recursively remove files under 'epoxyfsdata' directory using nftw().
void removeDir() {
  SERIAL_PORT_MONITOR.println("== Recursively remove '/' using nftw()");
  nftw("epoxyfsdata", removeFile, MAX_NOPEN_FD,
      FTW_PHYS | FTW_MOUNT | FTW_DEPTH);
}

#else

void removeDir() {
  SERIAL_PORT_MONITOR.println("== Recursively remove '/' not implemented");
}

#endif

void listDir() {
#if defined(ESP32)
  SERIAL_PORT_MONITOR.println("== List '/' not implemented on ESP32");

#else

  SERIAL_PORT_MONITOR.println("== List '/' using fs::Dir");

  // Open dir folder
  Dir dir = FILE_SYSTEM.openDir("/");

  int count = 1;
  // Cycle all the content
  while (dir.next()) {
    SERIAL_PORT_MONITOR.print("Dir entry #");
    SERIAL_PORT_MONITOR.println(count);

    // Print info from directory entry
    SERIAL_PORT_MONITOR.print("Dir: fileName()=");
    SERIAL_PORT_MONITOR.println(dir.fileName());
    if (dir.isDirectory()) {
      SERIAL_PORT_MONITOR.println("isDirectory=true");
    } else {
      SERIAL_PORT_MONITOR.println("isDirectory=false");
    }
    SERIAL_PORT_MONITOR.print("fileSize()=");
    SERIAL_PORT_MONITOR.println(dir.fileSize());

    // Print info from File object.
    SERIAL_PORT_MONITOR.print("File: ");
    File f = dir.openFile("r");
    SERIAL_PORT_MONITOR.print("name()=");
    SERIAL_PORT_MONITOR.println(f.name());
  #if defined(ESP32)
    SERIAL_PORT_MONITOR.print("path()=");
    SERIAL_PORT_MONITOR.println(f.path());
  #else
    SERIAL_PORT_MONITOR.print("fullName()=");
    SERIAL_PORT_MONITOR.println(f.fullName());
  #endif
    SERIAL_PORT_MONITOR.print("size()=");
    SERIAL_PORT_MONITOR.println(f.size());
    f.close();

    count++;
  }
#endif
}

void writeFile() {
  SERIAL_PORT_MONITOR.println("== Writing " TEST_FILE_NAME);

  File f = FILE_SYSTEM.open(TEST_FILE_NAME, "w");
  f.println("This is a test");
  f.println(42);
  f.println(42.0);
  f.println(42, 16);
  f.close();

  bool exists = FILE_SYSTEM.exists(TEST_FILE_NAME);
  if (exists) {
    SERIAL_PORT_MONITOR.println("Created " TEST_FILE_NAME);
  } else {
    SERIAL_PORT_MONITOR.println("ERROR creating " TEST_FILE_NAME);
  }
}

void readFile() {
  SERIAL_PORT_MONITOR.println("== Reading " TEST_FILE_NAME);

  File f = FILE_SYSTEM.open(TEST_FILE_NAME, "r");
  SERIAL_PORT_MONITOR.print("name(): ");
  SERIAL_PORT_MONITOR.println(f.name());
  #if defined(ESP32)
    SERIAL_PORT_MONITOR.print("path()=");
    SERIAL_PORT_MONITOR.println(f.path());
  #else
    SERIAL_PORT_MONITOR.print("fullName(): ");
    SERIAL_PORT_MONITOR.println(f.fullName());
  #endif
  while (f.available()) {
    String s = f.readStringUntil('\r');
    SERIAL_PORT_MONITOR.print(s);
    f.read();
    SERIAL_PORT_MONITOR.println();
  }
  f.close();
}

void setup() {
#if ! defined(EPOXY_DUINO)
  delay(1000); // some boards reboot twice
#endif

  SERIAL_PORT_MONITOR.begin(115200);
  while (!SERIAL_PORT_MONITOR); // For Leonardo/Micro
#if defined(EPOXY_DUINO)
  SERIAL_PORT_MONITOR.setLineModeUnix();
#endif

  SERIAL_PORT_MONITOR.println(F("== Initializing " FILE_SYSTEM_NAME));
  if (! FILE_SYSTEM.begin()) {
    SERIAL_PORT_MONITOR.println(F("ERROR initializing file system."));
    exit(1);
  }

  SERIAL_PORT_MONITOR.println(F("== Formatting file system"));
  if (! FILE_SYSTEM.format()) {
    SERIAL_PORT_MONITOR.println(F("ERROR formatting file system."));
    exit(1);
  }

  listDir();
  writeFile();
  listDir();
  readFile();
  removeDir();
  listDir();

  SERIAL_PORT_MONITOR.println(F("== Done"));

#if defined(EPOXY_DUINO)
  exit(0);
#endif
}

void loop() {
}
