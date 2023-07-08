#ifdef NO_GLOBAL_INSTANCES
#include <logger.h>
#endif

#include "Commands/CDUP.h"
#include "Commands/CWD.h"
#include "Commands/DELE.h"
#include "Commands/LIST.h"
#include "Commands/MKD.h"
#include "Commands/MLSD.h"
#include "Commands/NLST.h"
#include "Commands/PORT.h"
#include "Commands/PWD.h"
#include "Commands/RETR.h"
#include "Commands/RMD.h"
#include "Commands/RNFR_RNTO.h"
#include "Commands/STAT.h"
#include "Commands/STOR.h"
#include "Commands/TYPE.h"
#include "ESP-FTP-Server-Lib.h"
#include "common.h"

FTPConnection::FTPConnection(const WiFiClient &Client, std::list<FTPUser> &UserList, FTPFilesystem &Filesystem) : _ClientState(Idle), _Client(Client), _Filesystem(Filesystem), _UserList(UserList), _AuthUsername("") {
  std::shared_ptr<FTPCommandTransfer> retr = std::shared_ptr<FTPCommandTransfer>(new RETR(&_Client, &_Filesystem, &_DataAddress, &_DataPort));
  std::shared_ptr<FTPCommandTransfer> stor = std::shared_ptr<FTPCommandTransfer>(new STOR(&_Client, &_Filesystem, &_DataAddress, &_DataPort));

  _FTPCommands.push_back(std::shared_ptr<FTPCommand>(new CDUP(&_Client)));
  _FTPCommands.push_back(std::shared_ptr<FTPCommand>(new CWD(&_Client, &_Filesystem)));
  _FTPCommands.push_back(std::shared_ptr<FTPCommand>(new DELE(&_Client, &_Filesystem)));
  _FTPCommands.push_back(std::shared_ptr<FTPCommand>(new STAT(&_Client, &_Filesystem)));
  _FTPCommands.push_back(std::shared_ptr<FTPCommand>(new LIST(&_Client, &_Filesystem, &_DataAddress, &_DataPort)));
  _FTPCommands.push_back(std::shared_ptr<FTPCommand>(new NLST(&_Client, &_Filesystem, &_DataAddress, &_DataPort)));
  _FTPCommands.push_back(std::shared_ptr<FTPCommand>(new MLSD(&_Client, &_Filesystem, &_DataAddress, &_DataPort)));
  _FTPCommands.push_back(std::shared_ptr<FTPCommand>(new MKD(&_Client, &_Filesystem)));
  _FTPCommands.push_back(std::shared_ptr<FTPCommand>(new PORT(&_Client, &_DataAddress, &_DataPort)));
  _FTPCommands.push_back(std::shared_ptr<FTPCommand>(new PWD(&_Client)));
  _FTPCommands.push_back(retr);
  _FTPCommands.push_back(std::shared_ptr<FTPCommand>(new RMD(&_Client, &_Filesystem)));
  _FTPCommands.push_back(std::shared_ptr<FTPCommand>(new RNFR_RNTO(&_Client, &_Filesystem)));
  _FTPCommands.push_back(stor);
  _FTPCommands.push_back(std::shared_ptr<FTPCommand>(new TYPE(&_Client)));

  _FTPCommandsTransfer.push_back(retr);
  _FTPCommandsTransfer.push_back(stor);

#ifndef NO_GLOBAL_INSTANCES
  Serial.print("New Connection from ");
  Serial.print(_Client.remoteIP());
  Serial.print(":");
  Serial.println(_Client.remotePort());
#else
  logPrintI("New Connection from ");
  logPrintI(_Client.remoteIP().toString());
  logPrintI(":");
  logPrintlnI(String(_Client.remotePort()));
#endif
  _Client.println("220--- Welcome to FTP Server for ESP32 ---");
  _Client.println("220---        By Peter Buchegger       ---");
  _Client.println("220 --           Version 0.1           ---");
}

FTPConnection::~FTPConnection() {
#ifndef NO_GLOBAL_INSTANCES
  Serial.println("Connection closed!");
#else
  logPrintlnI("Connection closed!");
#endif
}

bool FTPConnection::readUntilLineEnd() {
  while (_Client.available()) {
    char c = _Client.read();
    if (c == '\n') {
      uint32_t index_separator = _Line.indexOf(' ');
      _LineSplited             = {_Line.substring(0, index_separator), _Line.substring(index_separator + 1, _Line.length())};
      return true;
    }
    if (c >= 32) {
      _Line += c;
    }
  }
  return false;
}

bool FTPConnection::handle() {
  if (!_Client.connected()) {
    return false;
  }
  for (std::shared_ptr<FTPCommandTransfer> cmd : _FTPCommandsTransfer) {
    if (cmd->trasferInProgress()) {
      cmd->workOnData();
    }
  }
  if (!readUntilLineEnd()) {
    return true;
  }
  // we have a new command in the queue:
#ifndef NO_GLOBAL_INSTANCES
  Serial.println(_Line);
#else
  logPrintlnD(_Line);
#endif
  String command = _LineSplited[0];

  // This commands are always possible:
  if (command == "SYST") {
    _Client.println("215 UNIX Type: L8");
    _Line = "";
    return true;
  }
  /**    Additional commads begin *************************************  by Akoro */
  else if (command == "OPTS") // need for Win10 ftp
  {
    if (_LineSplited[1] == "UTF8 ON") {
      _Client.println("200 Ok");
    } else {
      _Client.println("500 not implemented");
    }
    _Line = "";
    return true;
  } else if (command == "NOOP") {
    _Client.println("200 Ok");
    _Line = "";
    return true;
  } else if (command == "FEAT") {
    _Client.println("211- Extensions suported:");
    _Client.println(" UTF8");
    _Client.println(" MLSD");
    _Client.println("211 End.");
    _Line = "";
    return true;
  }
  /**    Additional commads end ************************************* by Akoro */
  else if (command == "QUIT") {
    _Client.println("221 Goodbye");
    _Client.stop();
    _Line = "";
    return true;
  } else if (command == "ABOR") {
    for (std::shared_ptr<FTPCommandTransfer> cmd : _FTPCommandsTransfer) {
      cmd->abort();
    }
    _Client.println("226 Data connection closed");
    _Line = "";
    return true;
  }

  // Logged in?
  switch (_ClientState) {
  case Idle:
    if (command == "USER") {
      c_USER();
    } else {
      _Client.println("500 Unknown command");
#ifndef NO_GLOBAL_INSTANCES
      Serial.println("USER: 500 Unknown command");
#else
      logPrintlnW("USER: 500 Unknown command");
#endif
    }
    break;

  case UsernamePass:
    if (command == "PASS") {
      c_PASS();
    } else {
      _Client.println("500 Unknown command");
#ifndef NO_GLOBAL_INSTANCES
      Serial.println("PASS: 500 Unknown command");
#else
      logPrintlnW("PASS: 500 Unknown command");
#endif
    }
    break;

  case AuthPass: {
    std::vector<std::shared_ptr<FTPCommand>>::iterator cmdIter = std::find_if(_FTPCommands.begin(), _FTPCommands.end(), [&](std::shared_ptr<FTPCommand> cmd) {
      if (command == cmd->getName() || (cmd->getName() == "RN" && (command == "RNFR" || command == "RNTO"))) {
        return true;
      }
      return false;
    });
    if (cmdIter != _FTPCommands.end()) {
      (*cmdIter)->run(_WorkDirectory, _LineSplited);
    } else {
      _Client.println("500 Unknown command");
#ifndef NO_GLOBAL_INSTANCES
      Serial.println("500 Unknown command");
#else
      logPrintlnW("500 Unknown command");
#endif
    }
  }

  default:
    break;
  }
  _Line = "";
  return true;
}

bool FTPConnection::connected() {
  return _Client.connected();
}

void FTPConnection::c_USER() {
  String                       username = _LineSplited[1];
  std::list<FTPUser>::iterator userIter = std::find_if(_UserList.begin(), _UserList.end(), [&](const FTPUser &user) {
    if (username == user.Username) {
      return true;
    }
    return false;
  });
  if (userIter != _UserList.end()) {
    _AuthUsername = username;
    _Client.println("331 OK. Password required");
    _ClientState = UsernamePass;
    return;
  }
  _Client.println("530 user not found");
}

void FTPConnection::c_PASS() {
  String                       password = _LineSplited[1];
  String                       username = _AuthUsername;
  std::list<FTPUser>::iterator _user    = std::find_if(_UserList.begin(), _UserList.end(), [&](const FTPUser &user) {
    if (username == user.Username && password == user.Password) {
      return true;
    }
    return false;
  });
  if (_user != _UserList.end()) {
    _Client.println("230 OK.");
    _ClientState = AuthPass;
    return;
  }
  _Client.println("530 password not correct");
}
