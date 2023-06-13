#ifndef STAT_H_
#define STAT_H_

#include <WiFiClient.h>

#include "../FTPCommand.h"
#include "../common.h"

class STAT : public FTPCommand {
public:
  explicit STAT(WiFiClient *const Client, FTPFilesystem *const Filesystem) : FTPCommand("STAT", 1, Client, Filesystem) {
  }

  void run(FTPPath &WorkDirectory, const std::vector<String> &Line) override {
    File dir = _Filesystem->open(WorkDirectory.getPath()); //
    if (!dir || !dir.isDirectory()) {
      SendResponse(550, "Can't open directory " + WorkDirectory.getPath());
      return;
    }
    int  cnt = 0;
    File f   = dir.openNextFile();
    while (f) {
      String filename = f.name();
      filename.remove(0, filename.lastIndexOf('/') + 1);
      if (f.isDirectory()) {
        data_print("drwxr-xr-x");
      } else {
        data_print("-rw-r--r--");
      }
      String filesize = String(f.size());
      data_print(" 1 owner group ");
      int fill_cnt = 13 - filesize.length();
      for (int i = 0; i < fill_cnt; i++) {
        data_print(" ");
      }
      data_println(filesize + " Jan 01  1970 " + filename);
      cnt++;
      f.close();
      f = dir.openNextFile();
    }
    SendResponse(226, String(cnt) + " matches total");
  }
};

#endif
