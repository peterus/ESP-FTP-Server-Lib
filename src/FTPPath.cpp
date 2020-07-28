#include "FTPPath.h"
#include "common.h"

FTPPath::FTPPath()
{
}

FTPPath::FTPPath(String path)
{
	changePath(path);
}

FTPPath::~FTPPath()
{
}

void FTPPath::changePath(String path)
{
	if(*path.begin() == '/')
	{
		_Path.clear();
	}
	std::vector<String> p = Split(path, '/');
	std::copy(p.begin(), p.end(), std::back_inserter(_Path));
}

void FTPPath::goPathUp()
{
	_Path.pop_back();
}

String FTPPath::getPath() const
{
	if(_Path.size() == 0)
	{
		return "/";
	}
	String path;
	for(const String & p: _Path)
	{
		path += "/";
		path += p;
	}
	return path;
}

String FTPPath::getFilePath(String filename) const
{
	if(*filename.begin() == '/')
	{
		return filename;
	}
	if(_Path.size() == 0)
	{
		return "/" + filename;
	}
	return getPath() + "/" + filename;
}
