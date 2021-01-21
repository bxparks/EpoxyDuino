#include <string>
#include "UnixHostFS.h"

using std::string;

namespace fs {

FS UnixHostFS(FSImplPtr(new UnixHostFSImpl()));

const char* rsflags(OpenMode openMode, AccessMode accessMode) {
  const char* mode;
  if (accessMode == AM_READ) {
    mode = "r";
  } else if (accessMode == AM_WRITE) {
    if (openMode == OM_TRUNCATE) {
      mode = "w";
    } else {
      mode = "a";
    }
  } else if (accessMode == AM_RW) {
    if (openMode == OM_DEFAULT) {
      mode = "r+";
    } else if (openMode == OM_TRUNCATE) {
      mode = "w+";
    } else {
      mode = "a+";
    }
  }
  return mode;
}

string fileNameConcat(const string& a, const string& b) {
  string merged = a;
  if (a[a.length() - 1] == '/') {
    if (b[0] == '/') {
      merged += (b.c_str() + 1);
    } else {
      merged += b;
    }
  } else {
    if (b[0] != '/') {
      merged += '/';
    }
    merged += b;
  }
  return merged;
}

}
