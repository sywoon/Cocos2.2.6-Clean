#include "FileUtils.h"
#include "cocoa/Dictionary.h"
#include "zip/unzip.h"
#include "file_xml/DictMaker.h"
#include "file_xml/tinyxml2.h"


NS_CC_BEGIN


FileUtils* FileUtils::_sharedFileUtils = NULL;


long FileUtils::getClassTypeId()
{
	static const long id = cocos2d::getHashCodeByString(typeid(cocos2d::FileUtils).name());
	return id;
}


void FileUtils::purgeFileUtils()
{
	CC_SAFE_DELETE(_sharedFileUtils);
}


FileUtils::FileUtils()
	: _pFilenameLookupDict(NULL)
{
}

FileUtils::~FileUtils()
{
	CC_SAFE_RELEASE(_pFilenameLookupDict);
}

bool FileUtils::init()
{
	_searchPathArray.push_back(_strDefaultResRootPath);
	return true;
}



unsigned char* FileUtils::getFileData(const char* pszFileName, const char* pszMode, unsigned long* pSize)
{
	unsigned char* pBuffer = NULL;
	CCAssert(pszFileName != NULL && pSize != NULL && pszMode != NULL, "Invalid parameters.");
	*pSize = 0;
	do
	{
		std::string fullPath = fullPathForFilename(pszFileName);
		FILE* fp = fopen(fullPath.c_str(), pszMode);
		CC_BREAK_IF(!fp);

		fseek(fp, 0, SEEK_END);
		*pSize = ftell(fp);
		fseek(fp, 0, SEEK_SET);
		pBuffer = new unsigned char[*pSize];
		*pSize = fread(pBuffer, sizeof(unsigned char), *pSize, fp);
		fclose(fp);
	} while (0);

	if (!pBuffer)
	{
		CCLOG("Get data from file(%s) failed!", pszFileName);
	}
	return pBuffer;
}


unsigned char* FileUtils::getFileDataFromZip(const char* pszZipFilePath, const char* pszFileName, unsigned long* pSize)
{
	unsigned char* pBuffer = NULL;
	unzFile pFile = NULL;
	*pSize = 0;

	do
	{
		CC_BREAK_IF(!pszZipFilePath || !pszFileName);
		CC_BREAK_IF(strlen(pszZipFilePath) == 0);

		// 为什么不用 fullPathForFilename ？

		pFile = unzOpen(pszZipFilePath);
		CC_BREAK_IF(!pFile);

		int nRet = unzLocateFile(pFile, pszFileName, 1);
		CC_BREAK_IF(UNZ_OK != nRet);

		char szFilePathA[260];
		unz_file_info fileInfo;
		nRet = unzGetCurrentFileInfo(pFile, &fileInfo, szFilePathA, sizeof(szFilePathA), NULL, 0, NULL, 0);
		CC_BREAK_IF(UNZ_OK != nRet);

		nRet = unzOpenCurrentFile(pFile);
		CC_BREAK_IF(UNZ_OK != nRet);

		pBuffer = new unsigned char[fileInfo.uncompressed_size];
		int CC_UNUSED nSize = unzReadCurrentFile(pFile, pBuffer, fileInfo.uncompressed_size);
		CCAssert(nSize == 0 || nSize == (int)fileInfo.uncompressed_size, "the file size is wrong");

		*pSize = fileInfo.uncompressed_size;
		unzCloseCurrentFile(pFile);
	} while (0);

	if (pFile)
	{
		unzClose(pFile);
	}

	return pBuffer;
}

Dictionary* FileUtils::createDictionaryWithContentsOfFile(const std::string& filename)
{
	std::string fullPath = fullPathForFilename(filename.c_str());
	DictMaker tMaker;
	return tMaker.dictionaryWithContentsOfFile(fullPath.c_str());
}

bool FileUtils::writeDictionaryToFile(cocos2d::Dictionary* dict, const std::string& fullPath)
{
	tinyxml2::XMLDocument* pDoc = new tinyxml2::XMLDocument();
	if (NULL == pDoc)
		return false;

	tinyxml2::XMLDeclaration* pDeclaration = pDoc->NewDeclaration("xml version=\"1.0\" encoding=\"UTF-8\"");
	if (NULL == pDeclaration)
	{
		delete pDoc;
		return false;
	}

	pDoc->LinkEndChild(pDeclaration);
	tinyxml2::XMLElement* docType = pDoc->NewElement("!DOCTYPE plist PUBLIC \"-//Apple//DTD PLIST 1.0//EN\" \"http://www.apple.com/DTDs/PropertyList-1.0.dtd\"");
	pDoc->LinkEndChild(docType);

	tinyxml2::XMLElement* pRootEle = pDoc->NewElement("plist");
	pRootEle->SetAttribute("version", "1.0");
	if (NULL == pRootEle)
	{
		delete pDoc;
		return false;
	}
	pDoc->LinkEndChild(pRootEle);

	tinyxml2::XMLElement* innerDict = SAXParser::generateElementForDict(dict, pDoc);
	if (NULL == innerDict)
	{
		delete pDoc;
		return false;
	}
	pRootEle->LinkEndChild(innerDict);

	bool bRet = tinyxml2::XML_SUCCESS == pDoc->SaveFile(fullPath.c_str());

	delete pDoc;
	return bRet;
}

Array* FileUtils::createArrayWithContentsOfFile(const std::string& filename)
{
	std::string fullPath = fullPathForFilename(filename.c_str());
	DictMaker tMaker;
	return tMaker.arrayWithContentsOfFile(fullPath.c_str());
}


void FileUtils::setFilenameLookupDictionary(Dictionary* pFilenameLookupDict)
{
	_fullPathCache.clear();
	CC_SAFE_RELEASE(_pFilenameLookupDict);
	_pFilenameLookupDict = pFilenameLookupDict;
	CC_SAFE_RETAIN(_pFilenameLookupDict);
}

void FileUtils::loadFilenameLookupDictionaryFromFile(const char* filename)
{
	std::string fullPath = this->fullPathForFilename(filename);
	if (fullPath.length() > 0)
	{
		Dictionary* pDict = createDictionaryWithContentsOfFile(fullPath.c_str());
		if (pDict)
		{
			Dictionary* pMetadata = (Dictionary*)pDict->objectForKey("metadata");
			int version = ((String*)pMetadata->objectForKey("version"))->intValue();
			if (version != 1)
			{
				CCLOG("cocos2d: ERROR: Invalid filenameLookup dictionary version: %ld. Filename: %s", (long)version, filename);
				return;
			}
			setFilenameLookupDictionary((Dictionary*)pDict->objectForKey("filenames"));
		}
	}
}

std::string FileUtils::getRealFilename(const char* pszFileName)
{
	const char* pszNewFileName = NULL;

	String* fileNameFound = _pFilenameLookupDict ? 
					(String*)_pFilenameLookupDict->objectForKey(pszFileName) 
					: NULL;
	if (NULL == fileNameFound || fileNameFound->length() == 0) 
	{
		pszNewFileName = pszFileName;
	}
	else 
	{
		pszNewFileName = fileNameFound->getCString();
	}
	return pszNewFileName;
}


/////////////////////////////////
// search path
//

void FileUtils::setSearchPaths(const std::vector<std::string>& searchPaths)
{
	bool bExistDefaultRootPath = false;

	_fullPathCache.clear();
	_searchPathArray.clear();

	for (std::vector<std::string>::const_iterator iter = searchPaths.begin(); iter != searchPaths.end(); ++iter)
	{
		std::string strPrefix;
		std::string path;
		if (!isAbsolutePath(*iter))
		{
			strPrefix = _strDefaultResRootPath;
		}

		path = strPrefix + (*iter);
		if (path.length() > 0 && path[path.length() - 1] != '/')
		{
			path += "/";
		}

		if (!bExistDefaultRootPath && path == _strDefaultResRootPath)
		{
			bExistDefaultRootPath = true;
		}
		_searchPathArray.push_back(path);
	}

	if (!bExistDefaultRootPath)
	{
		_searchPathArray.push_back(_strDefaultResRootPath);
	}
}

const std::vector<std::string>& FileUtils::getSearchPaths()
{
	return _searchPathArray;
}

void FileUtils::addSearchPath(const char* path_)
{
	std::string strPrefix;
	std::string path(path_);
	if (!isAbsolutePath(path))
	{
		strPrefix = _strDefaultResRootPath;
	}

	path = strPrefix + path;
	if (path.length() > 0 && path[path.length() - 1] != '/')
	{
		path += "/";
	}
	_searchPathArray.push_back(path);
}

void FileUtils::removeSearchPath(const char* path_)
{
	std::string strPrefix;
	std::string path(path_);
	if (!isAbsolutePath(path))
	{
		strPrefix = _strDefaultResRootPath;
	}

	path = strPrefix + path;
	if (path.length() > 0 && path[path.length() - 1] != '/')
	{
		path += "/";
	}
	std::vector<std::string>::iterator iter = std::find(_searchPathArray.begin(), _searchPathArray.end(), path);
	_searchPathArray.erase(iter);
}

void FileUtils::removeAllPaths()
{
	_searchPathArray.clear();
}


std::string FileUtils::getPathForFilename(const std::string& filename, const std::string& searchPath)
{
	std::string path = searchPath + filename;
	if (!isFileExist(path)) {
		path = "";
	}
	return path;
}


std::string FileUtils::fullPathForFilename(const char* pszFileName)
{
	CCAssert(pszFileName != NULL, "CCFileUtils: Invalid path");

	std::string strFileName = pszFileName;
	if (isAbsolutePath(pszFileName))
	{
		return pszFileName;
	}

	std::map<std::string, std::string>::iterator cacheIter = _fullPathCache.find(pszFileName);
	if (cacheIter != _fullPathCache.end())
	{
		return cacheIter->second;
	}

	std::string realFilename = getRealFilename(pszFileName);
	std::string fullpath = "";

	for (std::vector<std::string>::iterator searchPathsIter = _searchPathArray.begin();
		searchPathsIter != _searchPathArray.end(); ++searchPathsIter) 
	{
		fullpath = this->getPathForFilename(realFilename, *searchPathsIter);

		if (fullpath.length() > 0)
		{
			_fullPathCache.insert(std::pair<std::string, std::string>(pszFileName, fullpath));
			return fullpath;
		}
	}

	return pszFileName;
}

void FileUtils::purgeFullPathCached()
{
	_fullPathCache.clear();
}

bool FileUtils::isAbsolutePath(const std::string& strPath)
{
	return strPath[0] == '/' ? true : false;
}

const char* FileUtils::fullPathFromRelativeFile(const char* pszFilename, const char* pszRelativeFile)
{
	std::string relativeFile = pszRelativeFile;
	String* pRet = String::create("");

	pRet->_sString = relativeFile.substr(0, relativeFile.rfind('/') + 1);
	pRet->_sString += getRealFilename(pszFilename);
	return pRet->getCString();
}



NS_CC_END
