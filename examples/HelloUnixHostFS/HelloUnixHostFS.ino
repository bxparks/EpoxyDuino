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
  // Open dir folder
  Dir dir = filesystem.openDir("./");
  // Cycle all the content
  while (dir.next()) {
    // Print file
    SERIAL_PORT_MONITOR.print(dir.fileName());
    if (dir.isDirectory()) {
      SERIAL_PORT_MONITOR.println('/');
    } else {
      SERIAL_PORT_MONITOR.println();
    }

    /*
    // If element have a size display It else write 0
    if (dir.fileSize()) {
        File f = dir.openFile("r");
        SERIAL_PORT_MONITOR.println(f.size());
        f.close();
    } else {
        SERIAL_PORT_MONITOR.println("0");
    }
    */
  }
}

void setup() {
#if ! defined(UNIX_HOST_DUINO)
  delay(1000); // some boards reboot twice
#endif

  SERIAL_PORT_MONITOR.begin(115200);
  while (!SERIAL_PORT_MONITOR); // For Leonardo/Micro

#if defined(UNIX_HOST_DUINO)
  SERIAL_PORT_MONITOR.println(F("Inizializing UnixHostFS..."));
  FS& filesystem = UnixHostFS;
#else
  SERIAL_PORT_MONITOR.println(F("Inizializing LittleFS..."));
  FS& filesystem = LittleFS;
#endif

  if (filesystem.begin()){
      SERIAL_PORT_MONITOR.println(F("done."));
  } else {
      SERIAL_PORT_MONITOR.println(F("fail."));
  }

  listDir(filesystem);

#if defined(UNIX_HOST_DUINO)
  exit(0);
#endif
}

void loop() {
}
