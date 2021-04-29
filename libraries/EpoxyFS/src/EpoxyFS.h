#ifndef EPOXY_FS_H
#define EPOXY_FS_H

#include <string> // std::string
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

/** Concatenate 2 file components. */
std::string fileNameConcat(const std::string& a, const std::string& b);

class EpoxyFileImpl: public FileImpl {
  public:
    EpoxyFileImpl(
        const char* fsroot,
        const std::string& path,
        const char* mode,
        FILE* fp
    ) :
        fsroot_(fsroot),
        path_(path),
        mode_(mode),
        fp_(fp)
    {
      // Extract file info
      int fd = fileno(fp_);
      ::fstat(fd, &stat_);

      // Extract the basename of path
      size_t pos = path_.find_last_of("/");
      if (pos == std::string::npos) {
        name_ = path;
      } else {
        name_ = path_.substr(pos + 1, std::string::npos);
      }
    }

    ~EpoxyFileImpl() override {
      close();
    }

    size_t write(const uint8_t *buf, size_t size) override {
      return ::fwrite(buf, 1, size, fp_);
    }

    size_t read(uint8_t* buf, size_t size) override {
      return ::fread(buf, 1, size, fp_);
    }

    void flush() override {
      fflush(fp_);
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
      ::fseek(fp_, pos, whence);
      return true;
    }

    size_t position() const override {
      return ::ftell(fp_);
    }

    size_t size() const override {
      return stat_.st_size;
    }

    // There are 2 Unix truncate functions, truncate(path, ...) and
    // ftruncate(fd, ...), neither take the (FILE*) pointer. I tried truncating
    // it with the fd descriptor, but I couldn't get it to work. Truncating
    // using the full filePath works, but I have to close the FILE* buffer.
    // Which means that I have to reopen the file. If there is an easier way of
    // doing this, I'd be happy to learn it.
    bool truncate(uint32_t size) override {
      close();
      std::string unixPath = fileNameConcat(fsroot_, path_);
      int status = ::truncate(unixPath.c_str(), size);
      if (status == 0) {
        reopen();
      }
      return status == 0;
    }

    void close() override {
      if (fp_) {
        ::fclose(fp_);
        fp_ = nullptr;
      }
    }

    const char* name() const override {
      return name_.c_str();
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

    void reopen() {
      std::string unixPath = fileNameConcat(fsroot_, path_);
      fp_ = ::fopen(unixPath.c_str(), mode_);
      int fd = fileno(fp_);
      ::fstat(fd, &stat_);
    }

    const char* const fsroot_;
    const std::string path_;
    const char* const mode_;
    FILE* fp_;

    std::string name_;
    struct stat stat_;
};

class EpoxyDirImpl: public DirImpl {
  public:
    EpoxyDirImpl(const char* fsroot, const std::string& path)
      : fsroot_(fsroot),
        path_(path)
    {
      std::string unixPath = fileNameConcat(fsroot, path);
      dir_ = ::opendir(unixPath.c_str());
    }

    ~EpoxyDirImpl() override {
      if (dir_) {
        ::closedir(dir_);
        dir_ = nullptr;
      }
    }

    FileImplPtr openFile(OpenMode openMode, AccessMode accessMode) override {
      const char* mode = rsflags(openMode, accessMode);
      std::string filePath = fileNameConcat(path_, fileName());
      std::string unixPath = fileNameConcat(fsroot_, filePath);
      FILE* fp = ::fopen(unixPath.c_str(), mode);
      if (fp) {
        return std::make_shared<EpoxyFileImpl>(fsroot_, filePath, mode, fp);
      } else {
        return nullptr;
      }
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
    //
    // By default, FS doesn't report file times
    time_t fileTime() override { return 0; }

    // By default, FS doesn't report file times
    time_t fileCreationTime() override { return 0; }

    bool isFile() const override {
      return dirEntry_->d_type == DT_REG;
    }

    bool isDirectory() const override {
      return dirEntry_->d_type == DT_DIR;
    }

    bool next() override {
      // Skip over the Unix "." and ".." directories which don't exist on
      // LittleFS or SPIFFS.
      while (true) {
        dirEntry_ = ::readdir(dir_);
        if (dirEntry_ == nullptr) return false;
        if (strcmp(fileName(), ".") != 0
            && strcmp(fileName(), "..") != 0) break;
      }
      // TODO: Do I need to recreate the full path if this file is
      // under a subdirectory?
      ::lstat(fileName(), &stat_);
      return dirEntry_ != nullptr;
    }

    bool rewind() override {
      ::rewinddir(dir_);
      return true;
    }

  private:
    const char* const fsroot_;
    const std::string path_;

    DIR* dir_;
    struct dirent* dirEntry_;
    struct stat stat_;
};

class EpoxyFSImpl: public FSImpl {
  public:
    ~EpoxyFSImpl() override { }

    bool setConfig(const FSConfig &/*cfg*/) override {
      // TODO: Implement me
      return true;
    }

    bool begin() override;

    void end() override {
    }

    bool format() override;

    bool info(FSInfo& /*info*/) override {
      // TODO: Implement me
      return true;
    }

    bool info64(FSInfo64& /*info*/) override {
      // TODO: Implement me
      return true;
    }

    FileImplPtr open(
        const char* path,
        OpenMode openMode,
        AccessMode accessMode
    ) override {
      std::string unixPath = fileNameConcat(fsroot_, path);
      const char* mode = rsflags(openMode, accessMode);
      FILE* fp = ::fopen(unixPath.c_str(), mode);
      if (fp) {
        return std::make_shared<EpoxyFileImpl>(fsroot_, path, mode, fp);
      } else {
        return nullptr;
      }
    }

    bool exists(const char* path) override {
      struct stat stats;
      std::string unixPath = fileNameConcat(fsroot_, path);
      int status = ::lstat(unixPath.c_str(), &stats);
      return status == 0;
    }

    DirImplPtr openDir(const char* path) override {
      return std::make_shared<EpoxyDirImpl>(fsroot_, path);
    }

    bool rename(const char* pathFrom, const char* pathTo) override {
      std::string unixPathFrom = fileNameConcat(fsroot_, pathFrom);
      std::string unixPathTo = fileNameConcat(fsroot_, pathTo);
      int status = ::rename(unixPathFrom.c_str(), unixPathTo.c_str());
      return status == 0;
    }

    bool remove(const char* path) override {
      std::string unixPath = fileNameConcat(fsroot_, path);
      int status = ::remove(unixPath.c_str());
      return status == 0;
    }

    // TODO: Does LittleFS recursivel create intermediate directories? If so
    // emulate that behavior in EpoxyFS. See
    // https://github.com/esp8266/Arduino/blob/master/libraries/LittleFS/src/LittleFS.cpp#L60
    // and
    // https://gist.github.com/JonathonReinhart/8c0d90191c38af2dcadb102c4e202950
    // for example.
    bool mkdir(const char* path) override {
      std::string unixPath = fileNameConcat(fsroot_, path);
      int status = ::mkdir(unixPath.c_str(), 0700);
      return status == 0;
    }

    // TODO: Does LittleFS refuse to delete a non-empty directory, or does it
    // force deletion of a non-empty directory?
    bool rmdir(const char* path) override {
      std::string unixPath = fileNameConcat(fsroot_, path);
      int status = ::rmdir(unixPath.c_str());
      return status == 0;
    }

  private:
    static const char* getFSRoot();

    const char* fsroot_;
};

extern FS EpoxyFS;

} // fs

#endif // EPOXY_FS_H
