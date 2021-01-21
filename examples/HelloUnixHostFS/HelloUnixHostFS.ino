#include <Arduino.h>

#if defined(UNIX_HOST_DUINO)
  #include <UnixHostFS.h>
#elif defined(ESP8266)
  #include <LittleFS.h>
#else
  #error Unsupported platform
#endif

using fs::UnixHostFS;

void listDir(FS& filesystem) {
  SERIAL_PORT_MONITOR.println("== Dir List '/'");

  // Open dir folder
  Dir dir = filesystem.openDir("/");
  // Cycle all the content
  while (dir.next()) {
    // Print directory entry
    SERIAL_PORT_MONITOR.print("Dir: ");
    SERIAL_PORT_MONITOR.print(dir.fileName());
    if (dir.isDirectory()) {
      SERIAL_PORT_MONITOR.println('/');
    } else {
      SERIAL_PORT_MONITOR.println();
    }

    // If element have a size display It else write 0
    SERIAL_PORT_MONITOR.print("File: ");
    if (dir.fileSize()) {
        File f = dir.openFile("r");
        SERIAL_PORT_MONITOR.print(f.name());
        SERIAL_PORT_MONITOR.print(':');
        SERIAL_PORT_MONITOR.println(f.size());
        f.close();
    } else {
        SERIAL_PORT_MONITOR.println("0");
    }
  }
}

void writeFile(FS& filesystem) {
  SERIAL_PORT_MONITOR.println("== Writing 'testfile.txt'");

  File f = filesystem.open("testfile.txt", "w");
  f.println("This is a test");
  f.println(42);
  f.println(42.0);
  f.println(42, 16);
  f.close();
}

void readFile(FS& filesystem) {
  SERIAL_PORT_MONITOR.println("== Reading 'testfile.txt'");

  File f = filesystem.open("testfile.txt", "r");
  while (f.available()) {
    String s = f.readStringUntil('\r');
    SERIAL_PORT_MONITOR.print(s);
    f.read();
    SERIAL_PORT_MONITOR.println();
  }
  f.close();
}

void setup() {
#if ! defined(UNIX_HOST_DUINO)
  delay(1000); // some boards reboot twice
#endif

  SERIAL_PORT_MONITOR.begin(115200);
  while (!SERIAL_PORT_MONITOR); // For Leonardo/Micro

#if defined(UNIX_HOST_DUINO)
  SERIAL_PORT_MONITOR.print(F("Inizializing UnixHostFS..."));
  FS& filesystem = UnixHostFS;
#else
  SERIAL_PORT_MONITOR.print(F("Inizializing LittleFS..."));
  FS& filesystem = LittleFS;
#endif

  if (filesystem.begin()){
    SERIAL_PORT_MONITOR.println(F("done."));
    listDir(filesystem);
    writeFile(filesystem);
    readFile(filesystem);
  } else {
    SERIAL_PORT_MONITOR.println(F("fail."));
  }

#if defined(UNIX_HOST_DUINO)
  exit(0);
#endif
}

void loop() {
}
