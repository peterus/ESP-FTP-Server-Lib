#ifndef ESP_FTP_LIB_H_
#define ESP_FTP_LIB_H_

#include <Arduino.h>
#include <list>

#include "FTPConnection.h"
#include "FTPFilesystem.h"
#include "FTPUser.h"

class FTPServer {
public:
  FTPServer();
  virtual ~FTPServer();

  void addUser(const String &Username, const String &Password);
  void addUser(const FTPUser &User);

  void addFilesystem(String Name, FS *const Filesystem);

  bool begin();
  void handle();

  size_t countConnections() const;

private:
  WiFiServer _Server;

  std::list<FTPUser>                        _UserList;
  std::list<std::shared_ptr<FTPConnection>> _Connections;

  FTPFilesystem _Filesystem;
};

#endif
