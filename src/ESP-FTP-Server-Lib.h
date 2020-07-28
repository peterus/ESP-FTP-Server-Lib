#ifndef ESP_FTP_LIB_H_
#define ESP_FTP_LIB_H_

#include <list>

#include <Arduino.h>
#include <FS.h>
#include "FTPUser.h"
#include "FTPConnection.h"

class FTPServer
{
public:
	FTPServer();
	virtual ~FTPServer();

	void addUser(const String & Username, const String & Password);
	void addUser(const FTPUser & User);

	void setFilesystem(FS * const Filesystem);

	bool begin();
	void handle();

private:
	WiFiServer _Server;

	std::list<FTPUser> _UserList;
	std::list<std::shared_ptr<FTPConnection>> _Connections;

	FS * _Filesystem;
};

#define FTP_DEBUG(txt) Serial.print("[DEBUG] "); Serial.println(txt)

#endif
