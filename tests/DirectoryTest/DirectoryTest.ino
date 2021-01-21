#line 2 "DirectoryTest.ino"

#include <Arduino.h>
#include <AUnit.h>

#if defined(UNIX_HOST_DUINO)
  #include <UnixHostFS.h>
#elif defined(ESP8266)
  #inclue <LittleFS.h>
#else
  #error Unsupported platform
#endif

using aunit::TestRunner;
using namespace fs;

//---------------------------------------------------------------------------

#if defined(UNIX_HOST_DUINO)
  FS& FILE_SYSTEM = UnixHostFS;
#elif defined(ESP8266)
  FS& FILE_SYSTEM = LittleFS;
#else
  #error Unsupported platform
#endif

//---------------------------------------------------------------------------

test(readEmptyDirectory_contains_dot_and_dotdot) {
  Dir dir = FILE_SYSTEM.openDir("/");
  int count = 0;
  while (dir.next()) {
    count++;
  }
  assertEqual(2, count);
}

//---------------------------------------------------------------------------

bool setupStatus = true;

void setup() {
#if ! defined(UNIX_HOST_DUINO)
  delay(1000); // wait to prevent garbage on SERIAL_PORT_MONITOR
#endif

  SERIAL_PORT_MONITOR.begin(115200);
  while (!SERIAL_PORT_MONITOR); // needed for Leonardo/Micro

  if (FILE_SYSTEM.begin()){
    SERIAL_PORT_MONITOR.println(F("FileSystem initialized."));
  } else {
    SERIAL_PORT_MONITOR.println(F("FileSystem init failed"));
    setupStatus = false;
  }

  // Start each test with a clean slate.
  FILE_SYSTEM.format();
}

void loop() {
  if (setupStatus) {
    TestRunner::run();
  }
}
