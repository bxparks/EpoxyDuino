#line 2 "EpoxyFSTest"

#include <Arduino.h>
#include <AUnit.h>
#include <AceCommon.h> // PrintStr

#if defined(EPOXY_DUINO)
  #include <EpoxyFS.h>
#elif defined(ESP8266)
  #include <LittleFS.h>
#else
  #error Unsupported platform
#endif

using ace_common::PrintStr;
using aunit::TestRunner;
using namespace fs;

//---------------------------------------------------------------------------

#if defined(EPOXY_DUINO)
  FS& FILE_SYSTEM = EpoxyFS;
#elif defined(ESP8266)
  FS& FILE_SYSTEM = LittleFS;
#else
  #error Unsupported platform
#endif

static const char FILE_NAME[] = "testfile.txt";
static const char FILE_PATH[] = "/testfile.txt";

static const char FILE_NAME2[] = "testfile2.txt";
static const char FILE_PATH2[] = "/testfile2.txt";
static const char TEXT[] = "This is a test";

static void writeFile(const char* fileName, const char* text) {
  File f = FILE_SYSTEM.open(fileName, "w");
  f.print(text);
  f.close();
}

static void readFileInto(File& f, Print& printStr) {
  while (f.available()) {
    int c = f.read();
    printStr.write(c);
  }
}

//---------------------------------------------------------------------------
// Tests for EpoxyDirImpl
//---------------------------------------------------------------------------

test(EpoxyDirImpl, next) {
  FILE_SYSTEM.format();
  writeFile(FILE_NAME, TEXT);

  Dir dir = FILE_SYSTEM.openDir("/");
  int count = 0;
  while (dir.next()) {
    count++;
  }

  assertEqual(1, count);
}

test(EpoxyDirImpl, rewind) {
  // Clear the filesystem, and create one file.
  FILE_SYSTEM.format();
  writeFile(FILE_NAME, TEXT);

  // Count number of entries, including the "." and "..".
  Dir dir = FILE_SYSTEM.openDir("/");
  int count = 0;
  while (dir.next()) {
    count++;
  }
  assertEqual(1, count);

  // Rewind the directory and count again.
  dir.rewind();
  while (dir.next()) {
    count++;
  }
  assertEqual(2, count);
}

//---------------------------------------------------------------------------
// Tests for EpoxyFileImpl
//---------------------------------------------------------------------------

test(EpoxyFileImplTest, writeFile_readFile) {
  writeFile(FILE_PATH, TEXT);

  File f = FILE_SYSTEM.open(FILE_PATH, "r");
  assertEqual(FILE_NAME, f.name());
  assertEqual(FILE_PATH, f.fullName());

  PrintStr<64> printStr;
  readFileInto(f, printStr);
  f.close();

  assertEqual(TEXT, printStr.getCstr());
}

test(EpoxyFileImplTest, seekFile) {
  writeFile(FILE_PATH, TEXT);

  File f = FILE_SYSTEM.open(FILE_PATH, "r");
  PrintStr<64> printStr;
  f.seek(5);
  readFileInto(f, printStr);
  f.close();

  assertEqual(F("is a test"), printStr.getCstr());
}

#if defined(EPOXY_DUINO)
// Seems to fail on actual ESP8266 using LittleFS:
// Panic lfs.c:3041 lfs_file_truncate: Assertion '(file->flags & 3) !=
// LFS_O_RDONLY' failed.
test(EpoxyFileImplTest, truncate) {
  writeFile(FILE_PATH, TEXT);

  File f = FILE_SYSTEM.open(FILE_PATH, "r");
  bool status = f.truncate(0);
  assertTrue(status);
  assertEqual((size_t) 0, f.size());
}
#endif

test(EpoxyFileImplTest, validateProperties) {
  writeFile(FILE_PATH, TEXT);

  File f = FILE_SYSTEM.open(FILE_PATH, "r");
  assertEqual(sizeof(TEXT) - 1, f.size());
  assertTrue(f.isFile());
  assertFalse(f.isDirectory());
  f.close();
}

//---------------------------------------------------------------------------
// Tests for EpoxyFSImpl
//---------------------------------------------------------------------------

test(EpoxyFSImplTest, exists) {
  writeFile(FILE_PATH, TEXT);
  assertTrue(FILE_SYSTEM.exists(FILE_PATH));
}

test(EpoxyFSImplTest, rename) {
  writeFile(FILE_PATH, TEXT);

  assertTrue(FILE_SYSTEM.exists(FILE_PATH));
  FILE_SYSTEM.rename(FILE_PATH, FILE_PATH2);
  assertFalse(FILE_SYSTEM.exists(FILE_PATH));
  assertTrue(FILE_SYSTEM.exists(FILE_PATH2));
}

test(EpoxyFSImplTest, remove) {
  writeFile(FILE_PATH, TEXT);

  assertTrue(FILE_SYSTEM.exists(FILE_PATH));
  FILE_SYSTEM.remove(FILE_PATH);
  assertFalse(FILE_SYSTEM.exists(FILE_PATH));
}

//---------------------------------------------------------------------------

bool setupStatus = true;

void setup() {
#if ! defined(EPOXY_DUINO)
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
  if (setupStatus) {
    FILE_SYSTEM.format();
  }
}

void loop() {
  if (setupStatus) {
    TestRunner::run();
  }
}
