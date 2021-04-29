#include <stdio.h> // remove()
#include <ftw.h> // nftw()
#include <string>
#include "EpoxyFS.h"

using std::string;

namespace fs {

FS EpoxyFS(FSImplPtr(new EpoxyFSImpl()));

//-----------------------------------------------------------------------------

const char* rsflags(OpenMode openMode, AccessMode accessMode) {
  const char* mode;
  if (accessMode == AM_READ) {
    mode = "r";
  } else if (accessMode == AM_WRITE) {
    if (openMode & OM_TRUNCATE) {
      mode = "w";
    } else {
      mode = "a";
    }
  } else if (accessMode == AM_RW) {
    if (openMode == OM_DEFAULT) {
      mode = "r+";
    } else if (openMode & OM_TRUNCATE) {
      mode = "w+";
    } else {
      mode = "a+";
    }
  } else {
    // Should never reach here unless AccessMode is modified.
    mode = "";
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

//-----------------------------------------------------------------------------

const char* EpoxyFSImpl::getFSRoot() {
  const char* envroot = getenv("EPOXY_FS_ROOT");
  if (envroot) {
    return envroot;
  } else {
    return "epoxyfsdata";
  }
}

bool EpoxyFSImpl::begin() {
  fsroot_ = getFSRoot();

  // Create the root directory if it does not exist.
  struct stat rootStats;
  int status = ::lstat(fsroot_, &rootStats);
  if (status != 0) {
    int mkdirStatus = ::mkdir(fsroot_, 0700);
    if (mkdirStatus != 0) return false;
    // Check the directory again
    status = ::lstat(fsroot_, &rootStats);
    if (status != 0) return false;
  }
  if (! S_ISDIR(rootStats.st_mode)) return false;
  return true;
}

static int removeFileOrDir(
    const char *fpath,
    const struct stat * /*sb*/,
    int /*typeflag*/,
    struct FTW *ftwbuf)
{
  /*
  if (typeflag == FTW_F) {
    printf("File: %s\n", fpath);
  } else if (typeflag == FTW_SL) {
    printf("Symlink: %s\n", fpath);
  } if (typeflag == FTW_DP) {
    printf("Post Dir: %s\n", fpath);
  }
  */
  int status = 0;
  if (ftwbuf->level != 0) {
    // Remove only children, not the top-level directory
    status = remove(fpath);
  }
  return status;
}

bool EpoxyFSImpl::format() {
  int status = nftw(fsroot_, removeFileOrDir, 5 /*nopenfd*/,
      FTW_PHYS | FTW_MOUNT | FTW_DEPTH);
  return status == 0;
}

} // fs
