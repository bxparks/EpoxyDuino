#include "FSImpl.h"

namespace fs {

// Moved from FS.cpp to here to allow unit testing. Could not move it to FS.h
// due to cyclic dependency with OpenMode and AccesMode.
bool sflags(const char* mode, OpenMode& om, AccessMode& am) {
    switch (mode[0]) {
        case 'r':
            am = AM_READ;
            om = OM_DEFAULT;
            break;
        case 'w':
            am = AM_WRITE;
            om = (OpenMode) (OM_CREATE | OM_TRUNCATE);
            break;
        case 'a':
            am = AM_WRITE;
            om = (OpenMode) (OM_CREATE | OM_APPEND);
            break;
        default:
            return false;
    }
    switch(mode[1]) {
        case '+':
            am = (AccessMode) (AM_WRITE | AM_READ);
            break;
        case 0:
            break;
        default:
            return false;
    }
    return true;
}

}
