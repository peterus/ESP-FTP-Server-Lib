#ifndef STOR_H_
#define STOR_H_

#include <WiFiClient.h>

#include "../FTPCommand.h"
#include "../common.h"

#define FTP_BUF_SIZE 4096

class STOR : public FTPCommandTransfer {
public:
  explicit STOR(WiFiClient *const Client, FTPFilesystem *const Filesystem, IPAddress *DataAddress, int *DataPort) : FTPCommandTransfer("STOR", 1, Client, Filesystem, DataAddress, DataPort) {
  }

  void run(FTPPath &WorkDirectory, const std::vector<String> &Line) override {
    if (trasferInProgress()) {
      return;
    }
    if (!ConnectDataConnection()) {
      return;
    }
    _ftpFsFilePath = WorkDirectory.getFilePath(Line[1]);
    _file          = _Filesystem->open(_ftpFsFilePath, "w");
    if (!_file) {
      SendResponse(451, "Can't open/create " + _ftpFsFilePath);
      CloseDataConnection();
      return;
    }
    workOnData();
  }

  void workOnData() override {
    uint8_t buffer[FTP_BUF_SIZE];
    int     nb = data_read(buffer, FTP_BUF_SIZE);
    if (nb > 0) {
      const auto wb = _file.write(buffer, nb);
      if (wb != nb) {
        _file.close();
        this->_Filesystem->remove(_ftpFsFilePath.c_str());

        SendResponse(552, "Error occured while STORing");
        CloseDataConnection();
      }

      return;
    }

    SendResponse(226, "File successfully transferred");
    CloseDataConnection();
    _file.close();
  }

private:
  String _ftpFsFilePath;
};

#endif
