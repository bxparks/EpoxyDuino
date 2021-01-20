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

class UnixHostFileImpl: public FileImpl {
  public:
    UnixHostFileImpl(const char* fileName, FILE* file) :
        fileName_(fileName),
        file_(file) {
      int fd = fileno(file);
      fstat(fd, &stat_);
    }

    ~UnixHostFileImpl() override {
      if (file_) {
        fclose(file_);
        file_ = nullptr;
      }
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
      fclose(file_);
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
      // Convert OpenMode and AccessMode to "r", "w", "r+", etc according to
      // the table in fopen(1).
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
      FILE* file = fopen(fileName(), mode);
      return std::make_shared<UnixHostFileImpl>(fileName(), file);
    }

    const char* fileName() override {
      return dirEntry_->d_name;
    }

    size_t fileSize() override {
      return 0;
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
      return dirEntry_ != nullptr;
    }

    bool rewind() override {
      return true;
    }

  private:
    DIR* dir_;
    struct dirent* dirEntry_;
};

class UnixHostFSImpl: public FSImpl {
  public:
    ~UnixHostFSImpl() override { }

    bool setConfig(const FSConfig &cfg) override {
      return true;
    }

    bool begin() override {
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
      return 0;
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
};

extern FS UnixHostFS;

} // fs

#endif // UNIX_HOST_FS_H
