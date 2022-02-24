#line 2 "EpoxyFSTest"

#include <Arduino.h>
#include <AUnit.h>
#include <AceCommon.h> // PrintStr

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

using ace_common::PrintStr;
using aunit::TestRunner;
using namespace fs;

//---------------------------------------------------------------------------

static const char FILE_NAME[] = "testfile.txt";
static const char FILE_PATH[] = "/testfile.txt";

//static const char FILE_NAME2[] = "testfile2.txt";
static const char FILE_PATH2[] = "/testfile2.txt";

//static const char FILE_NAME_DOES_NOT_EXIST[] = "doesnotexist.txt";
static const char FILE_PATH_DOES_NOT_EXIST[] = "/doesnotexist.txt";

//static const char DIR_NAME[] = "subdir";
static const char DIR_PATH[] = "/subdir";

//static const char SUBFILE_NAME[] = "subfile";
static const char SUBFILE_PATH []= "/subdir/subfile";

static const char TEXT[] = "This is a test";

static void writeFile(const char* filePath, const char* text) {
  File f = FILE_SYSTEM.open(filePath, "w");
  f.print(text);
  f.close();
}

static void readFileInto(File& f, Print& printStr) {
  while (f.available()) {
    int c = f.read();
    printStr.write(c);
  }
}

static int countDirEntries(const char* path) {
  Dir dir = FILE_SYSTEM.openDir(path);
  int count = 0;
  while (dir.next()) {
    count++;
  }
  return count;
}

//---------------------------------------------------------------------------
// Tests for EpoxyDirImpl
//---------------------------------------------------------------------------

test(EpoxyDirImpl, next) {
  writeFile(FILE_PATH, TEXT);
  assertEqual(1, countDirEntries("/"));

  // Clean up
  FILE_SYSTEM.remove(FILE_PATH);
}

test(EpoxyDirImpl, rewind) {
  writeFile(FILE_PATH, TEXT);

  // Count number of entries
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

  // Clean up
  FILE_SYSTEM.remove(FILE_PATH);
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

  // Clean up
  FILE_SYSTEM.remove(FILE_PATH);
}

test(EpoxyFileImplTest, seekFile) {
  writeFile(FILE_PATH, TEXT);

  File f = FILE_SYSTEM.open(FILE_PATH, "r");
  PrintStr<64> printStr;
  f.seek(5);
  readFileInto(f, printStr);
  f.close();

  assertEqual(F("is a test"), printStr.getCstr());

  // Clean up
  FILE_SYSTEM.remove(FILE_PATH);
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

  // Clean up
  FILE_SYSTEM.remove(FILE_PATH);
}
#endif

test(EpoxyFileImplTest, validateProperties) {
  writeFile(FILE_PATH, TEXT);

  File f = FILE_SYSTEM.open(FILE_PATH, "r");
  assertEqual(sizeof(TEXT) - 1, f.size());
  assertTrue(f.isFile());
  assertFalse(f.isDirectory());
  f.close();

  // Clean up
  FILE_SYSTEM.remove(FILE_PATH);
}

//---------------------------------------------------------------------------
// Tests for EpoxyFSImpl
//---------------------------------------------------------------------------

test(EpoxyFSImplTest, open_non_existing) {
  assertFalse(FILE_SYSTEM.exists(FILE_PATH_DOES_NOT_EXIST));
  File f = FILE_SYSTEM.open(FILE_PATH_DOES_NOT_EXIST, "r");
  assertFalse(!!f);
}

test(EpoxyFSImplTest, exists) {
  assertFalse(FILE_SYSTEM.exists(FILE_PATH_DOES_NOT_EXIST));

  writeFile(FILE_PATH, TEXT);
  assertTrue(FILE_SYSTEM.exists(FILE_PATH));

  // Clean up
  FILE_SYSTEM.remove(FILE_PATH);
}

test(EpoxyFSImplTest, rename) {
  writeFile(FILE_PATH, TEXT);
  assertTrue(FILE_SYSTEM.exists(FILE_PATH));

  FILE_SYSTEM.rename(FILE_PATH, FILE_PATH2);
  assertFalse(FILE_SYSTEM.exists(FILE_PATH));
  assertTrue(FILE_SYSTEM.exists(FILE_PATH2));

  // Clean up
  FILE_SYSTEM.remove(FILE_PATH2);
}

test(EpoxyFSImplTest, remove) {
  writeFile(FILE_PATH, TEXT);

  assertTrue(FILE_SYSTEM.exists(FILE_PATH));
  FILE_SYSTEM.remove(FILE_PATH);
  assertFalse(FILE_SYSTEM.exists(FILE_PATH));
}

test(EpoxyFSImplTest, mkdir_rmdir) {
  // Create directory at root level.
  bool status = FILE_SYSTEM.mkdir(DIR_PATH);
  assertTrue(status);
  assertEqual(1, countDirEntries("/"));
  assertEqual(0, countDirEntries(DIR_PATH));

  // Create a file under the subdirectory and verify its creation.
  writeFile(SUBFILE_PATH, TEXT);
  assertTrue(FILE_SYSTEM.exists(SUBFILE_PATH));
  assertEqual(1, countDirEntries("/"));  // root file count unchanged
  assertEqual(1, countDirEntries(DIR_PATH)); // subdir has one more file

  // Clean up
  FILE_SYSTEM.remove(SUBFILE_PATH);
  assertFalse(FILE_SYSTEM.exists(SUBFILE_PATH));
  assertEqual(0, countDirEntries(DIR_PATH));
  status = FILE_SYSTEM.rmdir(DIR_PATH);
  assertTrue(status);
}

//---------------------------------------------------------------------------
// Tests for sflags() and rsflags()
//---------------------------------------------------------------------------

#if defined(EPOXY_DUINO)

test(sflags_rsflags_round_trip) {
  OpenMode omode;
  AccessMode amode;

  assertTrue(sflags("r", omode, amode));
  assertEqual("r", rsflags(omode, amode));

  assertTrue(sflags("w", omode, amode));
  assertEqual("w", rsflags(omode, amode));

  assertTrue(sflags("a", omode, amode));
  assertEqual("a", rsflags(omode, amode));

  assertTrue(sflags("r+", omode, amode));
  assertEqual("r+", rsflags(omode, amode));

  assertTrue(sflags("w+", omode, amode));
  assertEqual("w+", rsflags(omode, amode));

  assertTrue(sflags("a+", omode, amode));
  assertEqual("a+", rsflags(omode, amode));

  assertFalse(sflags("invalid", omode, amode));
}

#endif // defined(EPOXY_DUINO)

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

    // To reduce wear, format only once at the start of the test suite.
    FILE_SYSTEM.format();
  } else {
    SERIAL_PORT_MONITOR.println(F("FileSystem init failed"));
    setupStatus = false;
  }
}

void loop() {
  if (setupStatus) {
    TestRunner::run();
  }
}
