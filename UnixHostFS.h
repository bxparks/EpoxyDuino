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

/**
 * Convert OpenMode and AccessMode to "r", "w", "r+", etc according to the
 * table in fopen(1). Essentially, this code reverses the mapping defined
 * by sflags() function in FS.cpp.
 */
const char* rsflags(OpenMode openMode, AccessMode accessMode);

/** Concatenate 2 file paths. */
std::string fileNameConcat(const std::string& a, const std::string& b);

class UnixHostFileImpl: public FileImpl {
  public:
    UnixHostFileImpl(const std::string& path, const char* mode)
      : path_(path),
        mode_(mode)
    {
      file_ = ::fopen(path_.c_str(), mode_);
      int fd = fileno(file_);
      ::fstat(fd, &stat_);
    }

    ~UnixHostFileImpl() override {
      close();
    }

    size_t write(const uint8_t *buf, size_t size) override {
      return ::fwrite(buf, 1, size, file_);
    }

    size_t read(uint8_t* buf, size_t size) override {
      return ::fread(buf, 1, size, file_);
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
      ::fseek(file_, pos, whence);
      return true;
    }

    size_t position() const override {
      return ::ftell(file_);
    }

    size_t size() const override {
      return stat_.st_size;
    }

    bool truncate(uint32_t size) override {
      return false;
    }

    void close() override {
      if (file_) {
        ::fclose(file_);
        file_ = nullptr;
      }
    }

    const char* name() const override {
      return path_.c_str();
    }

    const char* fullName() const override {
      return path_.c_str();
    }

    bool isFile() const override {
      return S_ISREG(stat_.st_mode);
    }

    bool isDirectory() const override {
      return S_ISDIR(stat_.st_mode);
    }

  private:
    const std::string path_;
    const char* const mode_;
    FILE* file_;
    struct stat stat_;
};

class UnixHostDirImpl: public DirImpl {
  public:
    UnixHostDirImpl(const std::string& path)
      : path_(path)
    {
      dir_ = ::opendir(path_.c_str());
    }

    ~UnixHostDirImpl() override {
      if (dir_) {
        ::closedir(dir_);
        dir_ = nullptr;
      }
    }

    FileImplPtr openFile(OpenMode openMode, AccessMode accessMode) override {
      const char* mode = rsflags(openMode, accessMode);
      std::string unixPath = fileNameConcat(path_, fileName());
      return std::make_shared<UnixHostFileImpl>(unixPath, mode);
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
      dirEntry_ = ::readdir(dir_);
      if (dirEntry_ != nullptr) {
        // TODO: Do I need to recreate the full path if this file is
        // under a subdirectory?
        ::lstat(fileName(), &stat_);
      }
      return dirEntry_ != nullptr;
    }

    bool rewind() override {
      return true;
    }

  private:
    const std::string path_;
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
      std::string unixPath = fileNameConcat(fsroot_, path);
      return std::make_shared<UnixHostFileImpl>(unixPath, mode);
    }

    bool exists(const char* path) override {
      return false;
    }

    DirImplPtr openDir(const char* path) override {
      std::string unixPath = fileNameConcat(fsroot_, path);
      return std::make_shared<UnixHostDirImpl>(unixPath);
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
