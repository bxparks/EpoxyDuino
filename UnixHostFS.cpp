#include "UnixHostFS.h"

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

}
