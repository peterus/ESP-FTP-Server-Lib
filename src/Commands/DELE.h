#ifndef DELE_H_
#define DELE_H_

#include <WiFiClient.h>
#include "../FTPCommand.h"

class DELE : public FTPCommand
{
public:
	DELE(WiFiClient * const Client, FS * const Filesystem) : FTPCommand("DELE", 1, Client, Filesystem) {}

	void run(FTPPath & WorkDirectory, const std::vector<String> & Line)
	{
		String filepath = WorkDirectory.getFilePath(Line[1]);
		if(!_Filesystem->exists(filepath))
		{
			SendResponse(550, "File " + filepath + " not found");
			return;
		}
		if(_Filesystem->remove(filepath))
		{
			SendResponse(250, " Deleted \"" + filepath + "\"");
		}
		else
		{
			SendResponse(450, "Can't delete \"" + filepath + "\"");
		}
	}
};

#endif
