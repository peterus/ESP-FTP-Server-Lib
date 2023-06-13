#ifndef NLST_H_
#define NLST_H_

#include <WiFiClient.h>

#include "../FTPCommand.h"
#include "../common.h"

class NLST : public FTPCommand {
public:
  explicit NLST(WiFiClient *const Client, FTPFilesystem *const Filesystem, IPAddress *DataAddress, int *DataPort) : FTPCommand("NLST", 1, Client, Filesystem, DataAddress, DataPort) {
  }

  void run(FTPPath &WorkDirectory, const std::vector<String> &Line) override {
    if (!ConnectDataConnection()) {
      return;
    }
    File dir = _Filesystem->open(WorkDirectory.getPath()); //
    if (!dir || !dir.isDirectory()) {
      CloseDataConnection();
      SendResponse(550, "Can't open directory " + WorkDirectory.getPath());
      return;
    }
    int  cnt = 0;
    File f   = dir.openNextFile();
    while (f) {
      String filename = f.name();
      filename.remove(0, filename.lastIndexOf('/') + 1);
      cnt++;
      f.close();
      f = dir.openNextFile();
    }
    CloseDataConnection();
    SendResponse(226, String(cnt) + " matches total");
  }
};

#endif
