#include <stdio.h> // remove()
#include <Arduino.h>

#if defined(EPOXY_DUINO)
  #include <ftw.h> // nftw()
  #include <EpoxyFS.h>
  #define FILE_SYSTEM fs::EpoxyFS
#elif defined(ESP8266)
  #include <LittleFS.h>
  #define FILE_SYSTEM LittleFS
#elif defined(ESP32)
  #include <LITTLEFS.h>
  #define FILE_SYSTEM LITTLEFS
#else
  #error Unsupported platform
#endif

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

void removeFtw() {
  SERIAL_PORT_MONITOR.println("== FTW List '/'");
  nftw("epoxyfsdata", removeFile, 5, FTW_PHYS | FTW_MOUNT | FTW_DEPTH);
}

#endif

void listDir() {
  SERIAL_PORT_MONITOR.println("== Dir List '/'");

  // Open dir folder
  Dir dir = FILE_SYSTEM.openDir("/");

  int count = 1;
  // Cycle all the content
  while (dir.next()) {
    SERIAL_PORT_MONITOR.print("Dir entry #");
    SERIAL_PORT_MONITOR.println(count);

    // Print info from directory entry
    SERIAL_PORT_MONITOR.print("Dir: fileName()=");
    SERIAL_PORT_MONITOR.print(dir.fileName());
    if (dir.isDirectory()) {
      SERIAL_PORT_MONITOR.print("; isDirectory=true");
    } else {
      SERIAL_PORT_MONITOR.print("; isDirectory=false");
    }
    SERIAL_PORT_MONITOR.print("; fileSize()=");
    SERIAL_PORT_MONITOR.println(dir.fileSize());

    // Print info from File object.
    SERIAL_PORT_MONITOR.print("File: ");
    File f = dir.openFile("r");
    SERIAL_PORT_MONITOR.print("name()=");
    SERIAL_PORT_MONITOR.print(f.name());
    SERIAL_PORT_MONITOR.print("; fullName()=");
    SERIAL_PORT_MONITOR.print(f.fullName());
    SERIAL_PORT_MONITOR.print("; size()=");
    SERIAL_PORT_MONITOR.println(f.size());
    f.close();

    count++;
  }
}

void writeFile() {
  SERIAL_PORT_MONITOR.println("== Writing 'testfile.txt'");

  File f = FILE_SYSTEM.open("testfile.txt", "w");
  f.println("This is a test");
  f.println(42);
  f.println(42.0);
  f.println(42, 16);
  f.close();

  bool exists = FILE_SYSTEM.exists("testfile.txt");
  if (exists) {
    SERIAL_PORT_MONITOR.println("'testfile.txt' created");
  } else {
    SERIAL_PORT_MONITOR.println("ERROR: 'testfile.txt' not created");
  }
}

void readFile() {
  SERIAL_PORT_MONITOR.println("== Reading '/testfile.txt'");

  File f = FILE_SYSTEM.open("/testfile.txt", "r");
  SERIAL_PORT_MONITOR.print("name(): ");
  SERIAL_PORT_MONITOR.println(f.name());
  SERIAL_PORT_MONITOR.print("fullName(): ");
  SERIAL_PORT_MONITOR.println(f.fullName());
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
  SERIAL_PORT_MONITOR.print(F("Inizializing EpoxyFS..."));
#elif define(ESP8266)
  SERIAL_PORT_MONITOR.print(F("Inizializing LittleFS..."));
#elif define(ESP32)
  SERIAL_PORT_MONITOR.print(F("Inizializing LITTLEFS..."));
#endif

  if (FILE_SYSTEM.begin()){
    SERIAL_PORT_MONITOR.println(F("done."));
    listDir();

#if defined(EPOXY_DUINO)
    writeFile();
    readFile();
    removeFtw();
#endif
  } else {
    SERIAL_PORT_MONITOR.println(F("fail."));
  }

#if defined(EPOXY_DUINO)
  exit(0);
#endif
}

void loop() {
}
