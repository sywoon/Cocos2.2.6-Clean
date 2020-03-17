#pragma once

#include "ccMacros.h"
#include "ccClassInfo.h"
#include <vector>
#include <string>
#include <map>


NS_CC_BEGIN


class Dictionary;
class CC_DLL FileUtils : public ClassInfo
{
public:
	virtual ~FileUtils();
	static FileUtils*		sharedFileUtils();  //在不同平台的子类中实现
	static void				purgeFileUtils();

public:
	virtual long			getClassTypeId();

	virtual unsigned char*	getFileData(const char* pszFileName, const char* pszMode, unsigned long* pSize);
	virtual unsigned char*	getFileDataFromZip(const char* pszZipFilePath, const char* pszFileName, unsigned long* pSize);

	virtual Dictionary*		createDictionaryWithContentsOfFile(const std::string& filename);
	virtual bool			writeDictionaryToFile(Dictionary* dict, const std::string& fullPath);

	virtual Array*			createArrayWithContentsOfFile(const std::string& filename);

	
	virtual void			loadFilenameLookupDictionaryFromFile(const char* filename);
	virtual void			setFilenameLookupDictionary(Dictionary* pFilenameLookupDict);
	virtual std::string		getRealFilename(const char* pszFileName);

	virtual void			setSearchPaths(const std::vector<std::string>& searchPaths);
	virtual const std::vector<std::string>& getSearchPaths();
	virtual void			addSearchPath(const char* path);
	virtual void			removeSearchPath(const char* path);

	void					removeAllPaths();


	virtual std::string		fullPathForFilename(const char* pszFileName);
	virtual void			purgeFullPathCached();

	// 不同平台会重写
	virtual bool			isAbsolutePath(const std::string& strPath);
	// 用pszRelativeFile所在的文件路径 + pszFilename
	virtual const char*		fullPathFromRelativeFile(const char* pszFilename, const char* pszRelativeFile);


protected:
	virtual std::string		getPathForFilename(const std::string& filename, const std::string& searchPath);


public:
	virtual bool			isFileExist(const std::string& strFilePath) = 0;
	virtual std::string		getWritablePath() = 0;



protected:
	FileUtils();

	virtual bool			init();


protected:
	static FileUtils*			_sharedFileUtils;

	Dictionary*					_pFilenameLookupDict;
	std::string					_strDefaultResRootPath;

	std::vector<std::string>	_searchPathArray;
	std::map<std::string, std::string> _fullPathCache;
};




NS_CC_END