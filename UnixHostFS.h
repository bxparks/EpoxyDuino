#ifndef UNIX_HOST_FS_H
#define UNIX_HOST_FS_H

#include <stdio.h> // fopen(), ftell(), etc
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h> // fstat()
#include <unistd.h>

#include "FS.h"
#include "FSImpl.h"

namespace fs {

/*
 * Convert OpenMode and AccessMode to "r", "w", "r+", etc according to the
 * table in fopen(1). Essentially, this code reverses the mapping defined
 * by sflags() function in FS.cpp.
 */
const char* rsflags(OpenMode openMode, AccessMode accessMode);

class UnixHostFileImpl: public FileImpl {
  public:
    UnixHostFileImpl(const char* fileName, FILE* file) :
        fileName_(fileName),
        file_(file) {
      int fd = fileno(file);
      fstat(fd, &stat_);
    }

    ~UnixHostFileImpl() override {
      close();
    }

    size_t write(const uint8_t *buf, size_t size) override {
      return 0;
    }

    size_t read(uint8_t* buf, size_t size) override {
      return 0;
    }

    void flush() override {
      fflush(file_);
    }

    bool seek(uint32_t pos, SeekMode mode) override {
      int whence;
      if (mode == SeekEnd) {
        whence = SEEK_END;
      } else if (mode == SeekCur) {
        whence = SEEK_CUR; 
      } else {
        whence = SEEK_SET;
      }
      fseek(file_, pos, whence);
      return true;
    }

    size_t position() const override {
      return ftell(file_);
    }

    size_t size() const override {
      return stat_.st_size;
    }

    bool truncate(uint32_t size) override {
      return false;
    }

    void close() override {
      if (file_) {
        fclose(file_);
        file_ = nullptr;
      }
    }

    const char* name() const override {
      return fileName_;
    }

    const char* fullName() const override {
      return fileName_;
    }

    bool isFile() const override {
      return S_ISREG(stat_.st_mode);
    }

    bool isDirectory() const override {
      return S_ISDIR(stat_.st_mode);
    }

  private:
    const char* fileName_;
    FILE* file_;
    struct stat stat_;
};

class UnixHostDirImpl: public DirImpl {
  public:
    UnixHostDirImpl(DIR* dir)
      : dir_(dir)
      {}

    ~UnixHostDirImpl() override {
      if (dir_) {
        closedir(dir_);
        dir_ = nullptr;
      }
    }

    FileImplPtr openFile(OpenMode openMode, AccessMode accessMode) override {
      const char* mode = rsflags(openMode, accessMode);
      FILE* file = fopen(fileName(), mode);
      return std::make_shared<UnixHostFileImpl>(fileName(), file);
    }

    const char* fileName() override {
      return dirEntry_->d_name;
    }

    size_t fileSize() override {
      return (dirEntry_) ? stat_.st_size : 0;
    }

    // Return the last written time for a file.  Undefined when called on a
    // writable file as the FS is allowed to return either the time of the last
    // write() operation or the time present in the filesystem metadata (often
    // the last time the file was closed)
    time_t fileTime() { return 0; } // By default, FS doesn't report file times

    // By default, FS doesn't report file times
    time_t fileCreationTime() { return 0; }

    bool isFile() const override {
      return dirEntry_->d_type == DT_REG;
    }

    bool isDirectory() const override {
      return dirEntry_->d_type == DT_DIR;
    }

    bool next() override {
      dirEntry_ = readdir(dir_);
      if (dirEntry_ != nullptr) {
        // TODO: Do I need to recreate the full path if this file is
        // under a subdirectory?
        lstat(fileName(), &stat_);
      }
      return dirEntry_ != nullptr;
    }

    bool rewind() override {
      return true;
    }

  private:
    DIR* dir_;
    struct dirent* dirEntry_;
    struct stat stat_;
};

class UnixHostFSImpl: public FSImpl {
  public:
    ~UnixHostFSImpl() override { }

    bool setConfig(const FSConfig &cfg) override {
      return true;
    }

    bool begin() override {
      const char* envroot = getenv("UNIX_HOST_FS_ROOT");
      if (envroot) {
        fsroot_ = envroot;
      } else {
        fsroot_ = "fsdata";
      }

      struct stat rootStats;
      int status = lstat(fsroot_, &rootStats);
      if (status != 0) {
        int mkdirStatus = ::mkdir(fsroot_, 0700);
        if (mkdirStatus != 0) return false;
        // Check the directory again
        status = lstat(fsroot_, &rootStats);
        if (status != 0) return false;
      }
      if (! S_ISDIR(rootStats.st_mode)) return false;
      return true;
    }

    void end() override {
    }

    bool format() override {
      return true;
    }

    bool info(FSInfo& info) override {
      return true;
    }

    bool info64(FSInfo64& info) override {
      return true;
    }

    FileImplPtr open(
        const char* path,
        OpenMode openMode,
        AccessMode accessMode
    ) override {
      const char* mode = rsflags(openMode, accessMode);
      FILE* file = fopen(path, mode);
      return std::make_shared<UnixHostFileImpl>(path, file);
    }

    bool exists(const char* path) override {
      return false;
    }

    DirImplPtr openDir(const char* path) override {
      DIR* dir = opendir(path);
      return std::make_shared<UnixHostDirImpl>(dir);
    }

    bool rename(const char* pathFrom, const char* pathTo) override {
      return false;
    }

    bool remove(const char* path) override {
      return false;
    }

    bool mkdir(const char* path) override {
      return false;
    }

    bool rmdir(const char* path) override {
      return false;
    }

  private:
    const char* fsroot_;
};

extern FS UnixHostFS;

} // fs

#endif // UNIX_HOST_FS_H
