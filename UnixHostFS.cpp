#include "UnixHostFS.h"

namespace fs {

FS UnixHostFS(FSImplPtr(new UnixHostFSImpl()));

}
