#ifndef FTP_FILESYSTEM_H_
#define FTP_FILESYSTEM_H_

#include <FS.h>
#include <FSImpl.h>
#include <map>

#include "FTPPath.h"

#ifndef FILE_READ
#define FILE_READ "r"
#endif

class FTPFilesystem {
public:
  FTPFilesystem();
  virtual ~FTPFilesystem();

  void addFilesystem(String Name, FS *const Filesystem);
  void clearFilesystemList();

  File open(const String &path, const char *mode = FILE_READ);
  bool exists(const String &path);
  bool remove(const String &path);
  bool rename(const String &pathFrom, const String &pathTo);
  bool mkdir(const String &path);
  bool rmdir(const String &path);

  void printFilesystems();

#ifndef UNIT_TEST
private:
#endif
  FS           *getFilesystem(String path);
  static String getPathWithoutFS(String path);

  std::map<String, FS *> _Filesystems;
};

class FTPFileImpl : public fs::FileImpl {
public:
  explicit FTPFileImpl(String name) : _Name(name) {
  }
  ~FTPFileImpl() {
  }
  size_t write(const uint8_t *buf, size_t size) override {
    return 0;
  };
#if defined(ESP32)
  size_t read(uint8_t *buf, size_t size) override {
    return 0;
  };
#elif defined(ESP8266)
  int read(uint8_t *buf, size_t size) override {
    return 0;
  };
#endif
  void flush() override{};
  bool seek(uint32_t pos, SeekMode mode) override {
    return false;
  };
  size_t position() const override {
    return 0;
  };
  size_t size() const override {
    return 0;
  };
  void   close() override{};
  time_t getLastWrite() override {
    return 0;
  };
#if defined ESP_IDF_VERSION && defined ESP_IDF_VERSION_VAL
#if ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(4, 0, 0)
  const char *path() const override {
    return "not implemented yet";
  };
#endif
#if ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(4, 4, 3)
  virtual boolean seekDir(long position) override {
    return false;
  }
  virtual String getNextFileName(void) override {
    return "";
  }
#if ESP_ARDUINO_VERSION >= ESP_ARDUINO_VERSION_VAL(2, 0, 8)
  virtual String getNextFileName(bool *isDir) override {
    *isDir = isDirectory();
    return getNextFileName();
  }
#endif
#endif
#if ESP_ARDUINO_VERSION >= ESP_ARDUINO_VERSION_VAL(2, 0, 3)
  bool setBufferSize(size_t size) override {
    return false;
  };
#endif
#endif
  const char *name() const override {
    return _Name.c_str();
  };
#if defined(ESP32)
  boolean isDirectory(void) override {
    return true;
  };
#elif defined(ESP8266)
  boolean isDirectory(void) const override {
    return true;
  };
  const char *fullName() const override {
    return ("/" + _Name).c_str();
  };
  bool isFile() const override {
    return true;
  };
  bool truncate(uint32_t size) override {
    return true;
  };
#endif

#if defined(ESP32)
  fs::FileImplPtr openNextFile(const char *mode) override
#elif defined(ESP8266)
  fs::FileImplPtr openNextFile(const char *mode)
#endif
  {
    if (_Filesystems.empty()) {
      return 0;
    }
    String next = _Filesystems.front();
    _Filesystems.pop_front();
    return fs::FileImplPtr(new FTPFileImpl(next));
  }

#if defined(ESP32)
  void rewindDirectory(void) override {
    return;
  };

  operator bool() override {
    return true;
  };
#endif

  void addFilesystem(String name) {
    _Filesystems.push_back(name);
  }

private:
  String            _Name;
  std::list<String> _Filesystems;
};

#endif
