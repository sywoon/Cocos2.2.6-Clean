#include "FileUtils.h"


NS_CC_BEGIN


class CC_DLL FileUtilsWin32 : public FileUtils
{
public:
	virtual bool			isAbsolutePath(const std::string& strPath);
	virtual std::string		fullPathForFilename(const char* pszFileName);

	virtual void			addSearchPath(const char* path);
	virtual void			removeSearchPath(const char* path);


public:
	virtual bool			isFileExist(const std::string& strFilePath);
	virtual std::string		getWritablePath();


protected:
	virtual std::string		getPathForFilename(const std::string& filename, const std::string& searchPath);
	std::string				utf8Togbk(const char* src);

protected:
	friend class FileUtils;
	FileUtilsWin32();

	virtual bool			init();
};



NS_CC_END