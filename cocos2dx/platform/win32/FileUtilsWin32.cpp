#include <Shlobj.h>
#include "FileUtilsWin32.h"
#include "CommonWin32.h"



using namespace std;

NS_CC_BEGIN

static char s_pszResourcePath[MAX_PATH] = { 0 };



FileUtils* FileUtils::sharedFileUtils()
{
	if (_sharedFileUtils == NULL)
	{
		_sharedFileUtils = new FileUtilsWin32();
		_sharedFileUtils->init();
	}
	return _sharedFileUtils;
}


FileUtilsWin32::FileUtilsWin32()
{
}

bool FileUtilsWin32::init()
{
	GetCurrentDirectory(sizeof(s_pszResourcePath), s_pszResourcePath);
	_strDefaultResRootPath = s_pszResourcePath;

	return FileUtils::init();
}

bool FileUtilsWin32::isAbsolutePath(const std::string& strPath)
{
	std::string strPathAscii = utf8Togbk(strPath.c_str());
	if (strPathAscii.length() > 2
		&& ((strPathAscii[0] >= 'a' && strPathAscii[0] <= 'z') || (strPathAscii[0] >= 'A' && strPathAscii[0] <= 'Z'))
		&& strPathAscii[1] == ':')
	{
		return true;
	}
	return false;
}

std::string FileUtilsWin32::fullPathForFilename(const char* pszFileName)
{
	std::string pszFileNameAscii = utf8Togbk(pszFileName);
	return FileUtils::fullPathForFilename(pszFileNameAscii.c_str());
}


void FileUtilsWin32::addSearchPath(const char* path)
{
	std::string strDes = utf8Togbk(path);
	FileUtils::addSearchPath(strDes.c_str());
}

void FileUtilsWin32::removeSearchPath(const char* path)
{
	std::string strDes = utf8Togbk(path);
	FileUtils::removeSearchPath(strDes.c_str());
}


bool FileUtilsWin32::isFileExist(const std::string& strFilePath)
{
	std::string strFilePathAscii = utf8Togbk(strFilePath.c_str());
	if (0 == strFilePathAscii.length())
		return false;

	std::string strPath = strFilePathAscii;
	if (!isAbsolutePath(strPath))
	{ // Not absolute path, add the default root path at the beginning.
		strPath.insert(0, _strDefaultResRootPath);
	}
	return GetFileAttributesA(strPath.c_str()) != -1 ? true : false;
}


string FileUtilsWin32::getWritablePath()
{
	// Get full path of executable, e.g. c:\Program Files (x86)\My Game Folder\MyGame.exe
	char full_path[_MAX_PATH + 1];
	::GetModuleFileNameA(NULL, full_path, _MAX_PATH + 1);

	// Debug app uses executable directory; Non-debug app uses local app data directory
#ifndef _DEBUG
		// Get filename of executable only, e.g. MyGame.exe
	char* base_name = strrchr(full_path, '\\');

	if (base_name)
	{
		char app_data_path[_MAX_PATH + 1];

		// Get local app data directory, e.g. C:\Documents and Settings\username\Local Settings\Application Data
		if (SUCCEEDED(SHGetFolderPathA(NULL, CSIDL_LOCAL_APPDATA, NULL, SHGFP_TYPE_CURRENT, app_data_path)))
		{
			string ret((char*)app_data_path);

			// Adding executable filename, e.g. C:\Documents and Settings\username\Local Settings\Application Data\MyGame.exe
			ret += base_name;

			// Remove ".exe" extension, e.g. C:\Documents and Settings\username\Local Settings\Application Data\MyGame
			ret = ret.substr(0, ret.rfind("."));

			ret += "\\";

			// Create directory
			if (SUCCEEDED(SHCreateDirectoryExA(NULL, ret.c_str(), NULL)))
			{
				return ret;
			}
		}
	}
#endif // not defined _DEBUG

	// If fetching of local app data directory fails, use the executable one
	string ret((char*)full_path);

	// remove xxx.exe
	ret = ret.substr(0, ret.rfind("\\") + 1);

	return ret;
}

std::string FileUtilsWin32::getPathForFilename(const std::string& filename, const std::string& searchPath)
{
	std::string filenameAscii = utf8Togbk(filename.c_str());
	std::string rearchPathAscii = utf8Togbk(searchPath.c_str());

	return FileUtils::getPathForFilename(filenameAscii, rearchPathAscii);
}





NS_CC_END