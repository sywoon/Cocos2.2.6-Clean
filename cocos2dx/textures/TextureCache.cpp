#include "TextureCache.h"
#include "cocoa/String.h"
#include "cocoa/Dictionary.h"
#include "pthread.h"
#include "platform/Image.h"
#include "Texture2D.h"
#include "Scheduler.h"
#include <queue>
#include <list>


NS_CC_BEGIN

typedef struct _AsyncStruct
{
	std::string     filename;
	Object*			target;
	SEL_CallFuncO   selector;
} AsyncStruct;

typedef struct _ImageInfo
{
	AsyncStruct* asyncStruct;
	Image*		 image;
	Image::EImageFormat imageType;
} ImageInfo;


static bool need_quit = false;
static pthread_t s_loadingThread;

static std::queue<AsyncStruct*>* s_pAsyncStructQueue = NULL;
static std::queue<ImageInfo*>* s_pImageQueue = NULL;

static pthread_mutex_t      s_asyncStructQueueMutex;
static pthread_mutex_t      s_ImageInfoMutex;

static pthread_mutex_t		s_SleepMutex;
static pthread_cond_t		s_SleepCondition;

static unsigned long		s_nAsyncRefCount = 0;




static TextureCache* g_sharedTextureCache = NULL;

TextureCache* TextureCache::sharedTextureCache()
{
	if (!g_sharedTextureCache)
	{
		g_sharedTextureCache = new TextureCache();
	}
	return g_sharedTextureCache;
}

void TextureCache::purgeSharedTextureCache()
{
	CC_SAFE_RELEASE_NULL(g_sharedTextureCache);
}


TextureCache::TextureCache()
{
	CCAssert(g_sharedTextureCache == NULL, "Attempted to allocate a second instance of a singleton.");

	_pTextures = new Dictionary();
}

TextureCache::~TextureCache()
{
	CCLOGINFO("cocos2d: deallocing CCTextureCache.");
	need_quit = true;
	pthread_cond_signal(&s_SleepCondition);
	CC_SAFE_RELEASE(_pTextures);
}

const char* TextureCache::description()
{
	return String::createWithFormat("<CCTextureCache | Number of textures = %u>", _pTextures->count())->getCString();
}


Texture2D* TextureCache::addImage(const char* path)
{
	CCAssert(path != NULL, "TextureCache: fileimage MUST not be NULL");

	Texture2D* texture = NULL;
	Image* pImage = NULL;

	std::string pathKey = path;

	pathKey = FileUtils::sharedFileUtils()->fullPathForFilename(pathKey.c_str());
	if (pathKey.size() == 0)
	{
		return NULL;
	}
	texture = (Texture2D*)_pTextures->objectForKey(pathKey.c_str());

	std::string fullpath = pathKey;
	if (!texture)
	{
		std::string lowerCase(pathKey);
		for (unsigned int i = 0; i < lowerCase.length(); ++i)
		{
			lowerCase[i] = tolower(lowerCase[i]);
		}
		// all images are handled by UIImage except PVR extension that is handled by our own handler
		do
		{
			if (std::string::npos != lowerCase.find(".pvr"))
			{
				texture = this->addPVRImage(fullpath.c_str());
			}
			else if (std::string::npos != lowerCase.find(".pkm"))
			{
				// ETC1 file format, only supportted on Android
				texture = this->addETCImage(fullpath.c_str());
			}
			else
			{
				Image::EImageFormat eImageFormat = Image::kFmtUnKnown;
				if (std::string::npos != lowerCase.find(".png"))
				{
					eImageFormat = Image::kFmtPng;
				}
				else if (std::string::npos != lowerCase.find(".jpg") || std::string::npos != lowerCase.find(".jpeg"))
				{
					eImageFormat = Image::kFmtJpg;
				}
				else if (std::string::npos != lowerCase.find(".tif") || std::string::npos != lowerCase.find(".tiff"))
				{
					eImageFormat = Image::kFmtTiff;
				}
				else if (std::string::npos != lowerCase.find(".webp"))
				{
					eImageFormat = Image::kFmtWebp;
				}

				pImage = new Image();
				CC_BREAK_IF(NULL == pImage);

				bool bRet = pImage->initWithImageFile(fullpath.c_str(), eImageFormat);
				CC_BREAK_IF(!bRet);

				texture = new Texture2D();

				if (texture && texture->initWithImage(pImage))
				{
#if CC_ENABLE_CACHE_TEXTURE_DATA
					// cache the texture file name
					VolatileTexture::addImageTexture(texture, fullpath.c_str(), eImageFormat);
#endif
					_pTextures->setObject(texture, pathKey.c_str());
					texture->release();
				}
				else
				{
					CCLOG("cocos2d: Couldn't create texture for file:%s in CCTextureCache", path);
				}
			}
		} while (0);
	}

	CC_SAFE_RELEASE(pImage);

	return texture;
}


Texture2D* TextureCache::addPVRImage(const char* path)
{
	CCAssert(path != NULL, "TextureCache: fileimage MUST not be nil");

	Texture2D* texture = NULL;
	std::string key(path);

	if ((texture = (Texture2D*)_pTextures->objectForKey(key.c_str())))
	{
		return texture;
	}

	// Split up directory and filename
	std::string fullpath = FileUtils::sharedFileUtils()->fullPathForFilename(key.c_str());
	texture = new Texture2D();
	if (texture != NULL && texture->initWithPVRFile(fullpath.c_str()))
	{
#if CC_ENABLE_CACHE_TEXTURE_DATA
		// cache the texture file name
		VolatileTexture::addImageTexture(texture, fullpath.c_str(), CCImage::kFmtRawData);
#endif
		_pTextures->setObject(texture, key.c_str());
		texture->autorelease();
	}
	else
	{
		CCLOG("cocos2d: Couldn't add PVRImage:%s in CCTextureCache", key.c_str());
		CC_SAFE_DELETE(texture);
	}

	return texture;
}

Texture2D* TextureCache::addETCImage(const char* path)
{
	CCAssert(path != NULL, "TextureCache: fileimage MUST not be nil");

	Texture2D* texture = NULL;
	std::string key(path);

	if ((texture = (Texture2D*)_pTextures->objectForKey(key.c_str())))
	{
		return texture;
	}

	// Split up directory and filename
	std::string fullpath = FileUtils::sharedFileUtils()->fullPathForFilename(key.c_str());
	texture = new Texture2D();
	if (texture != NULL && texture->initWithETCFile(fullpath.c_str()))
	{
		_pTextures->setObject(texture, key.c_str());
		texture->autorelease();
	}
	else
	{
		CCLOG("cocos2d: Couldn't add ETCImage:%s in CCTextureCache", key.c_str());
		CC_SAFE_DELETE(texture);
	}

	return texture;
}



static void loadImageData(AsyncStruct* pAsyncStruct)
{
	const char* filename = pAsyncStruct->filename.c_str();

	Image::EImageFormat imageType = Image::computeImageFormatType(pAsyncStruct->filename);
	if (imageType == Image::kFmtUnKnown)
	{
		CCLOG("unsupported format %s", filename);
		delete pAsyncStruct;
		return;
	}

	Image* pImage = new Image();
	if (pImage && !pImage->initWithImageFileThreadSafe(filename, imageType))
	{
		CC_SAFE_RELEASE(pImage);
		CCLOG("can not load %s", filename);
		return;
	}

	// generate image info
	ImageInfo* pImageInfo = new ImageInfo();
	pImageInfo->asyncStruct = pAsyncStruct;
	pImageInfo->image = pImage;
	pImageInfo->imageType = imageType;

	// put the image info into the queue
	pthread_mutex_lock(&s_ImageInfoMutex);
	s_pImageQueue->push(pImageInfo);
	pthread_mutex_unlock(&s_ImageInfoMutex);
}

static void* loadImage(void* data)
{
	AsyncStruct* pAsyncStruct = NULL;

	while (true)
	{
		std::queue<AsyncStruct*>* pQueue = s_pAsyncStructQueue;
		pthread_mutex_lock(&s_asyncStructQueueMutex);// get async struct from queue
		if (pQueue->empty())
		{
			pthread_mutex_unlock(&s_asyncStructQueueMutex);
			if (need_quit) {
				break;
			}
			else 
			{
				pthread_cond_wait(&s_SleepCondition, &s_SleepMutex);
				continue;
			}
		}
		else
		{
			pAsyncStruct = pQueue->front();
			pQueue->pop();
			pthread_mutex_unlock(&s_asyncStructQueueMutex);
			loadImageData(pAsyncStruct);
		}
	}

	if (s_pAsyncStructQueue != NULL)
	{
		delete s_pAsyncStructQueue;
		s_pAsyncStructQueue = NULL;
		delete s_pImageQueue;
		s_pImageQueue = NULL;

		pthread_mutex_destroy(&s_asyncStructQueueMutex);
		pthread_mutex_destroy(&s_ImageInfoMutex);
		pthread_mutex_destroy(&s_SleepMutex);
		pthread_cond_destroy(&s_SleepCondition);
	}

	return 0;
}


void TextureCache::addImageAsyncCallBack(float dt)
{
	// the image is generated in loading thread
	std::queue<ImageInfo*>* imagesQueue = s_pImageQueue;

	pthread_mutex_lock(&s_ImageInfoMutex);
	if (imagesQueue->empty())
	{
		pthread_mutex_unlock(&s_ImageInfoMutex);
	}
	else
	{
		ImageInfo* pImageInfo = imagesQueue->front();
		imagesQueue->pop();
		pthread_mutex_unlock(&s_ImageInfoMutex);

		AsyncStruct* pAsyncStruct = pImageInfo->asyncStruct;
		Image* pImage = pImageInfo->image;

		Object* target = pAsyncStruct->target;
		SEL_CallFuncO selector = pAsyncStruct->selector;
		const char* filename = pAsyncStruct->filename.c_str();

		// generate texture in render thread
		Texture2D* texture = new Texture2D();
		texture->initWithImage(pImage);

#if CC_ENABLE_CACHE_TEXTURE_DATA
		// cache the texture file name
		VolatileTexture::addImageTexture(texture, filename, pImageInfo->imageType);
#endif

		// cache the texture
		_pTextures->setObject(texture, filename);
		texture->autorelease();

		if (target && selector)
		{
			(target->*selector)(texture);
			target->release();
		}

		pImage->release();
		delete pAsyncStruct;
		delete pImageInfo;

		--s_nAsyncRefCount;
		if (0 == s_nAsyncRefCount)
		{
			Director::sharedDirector()->getScheduler()->unscheduleSelector(schedule_selector(TextureCache::addImageAsyncCallBack), this);
		}
	}
}


void TextureCache::addImageAsync(const char* path, Object* target, SEL_CallFuncO selector)
{
	CCAssert(path != NULL, "TextureCache: fileimage MUST not be NULL");

	Texture2D* texture = NULL;
	std::string pathKey = path;

	pathKey = FileUtils::sharedFileUtils()->fullPathForFilename(pathKey.c_str());
	texture = (Texture2D*)_pTextures->objectForKey(pathKey.c_str());

	std::string fullpath = pathKey;

	if (texture != NULL)
	{
		if (target && selector)
		{
			(target->*selector)(texture);
		}

		return;
	}

	// lazy init
	if (s_pAsyncStructQueue == NULL)
	{
		s_pAsyncStructQueue = new std::queue<AsyncStruct*>();
		s_pImageQueue = new std::queue<ImageInfo*>();

		pthread_mutex_init(&s_asyncStructQueueMutex, NULL);
		pthread_mutex_init(&s_ImageInfoMutex, NULL);
		pthread_mutex_init(&s_SleepMutex, NULL);
		pthread_cond_init(&s_SleepCondition, NULL);
		pthread_create(&s_loadingThread, NULL, loadImage, NULL);
		need_quit = false;
	}

	if (0 == s_nAsyncRefCount)
	{
		Director::sharedDirector()->getScheduler()->scheduleSelector(schedule_selector(TextureCache::addImageAsyncCallBack), this, 0, false);
	}

	++s_nAsyncRefCount;

	if (target)
	{
		target->retain();
	}

	// generate async struct
	AsyncStruct* data = new AsyncStruct();
	data->filename = fullpath.c_str();
	data->target = target;
	data->selector = selector;

	// add async struct into queue
	pthread_mutex_lock(&s_asyncStructQueueMutex);
	s_pAsyncStructQueue->push(data);
	pthread_mutex_unlock(&s_asyncStructQueueMutex);

	pthread_cond_signal(&s_SleepCondition);
}

Texture2D* TextureCache::addUIImage(Image* image, const char* key)
{
	CCAssert(image != NULL, "TextureCache: image MUST not be nil");

	Texture2D* texture = NULL;
	std::string forKey;
	if (key)
	{
		forKey = FileUtils::sharedFileUtils()->fullPathForFilename(key);
	}

	// Don't have to lock here, because addImageAsync() will not 
	// invoke opengl function in loading thread.
	do
	{
		// If key is nil, then create a new texture each time
		if (key && (texture = (Texture2D*)_pTextures->objectForKey(forKey.c_str())))
		{
			break;
		}

		// prevents overloading the autorelease pool
		texture = new Texture2D();
		texture->initWithImage(image);

		if (key && texture)
		{
			_pTextures->setObject(texture, forKey.c_str());
			texture->autorelease();
		}
		else
		{
			CCLOG("cocos2d: Couldn't add UIImage in CCTextureCache");
		}

	} while (0);

	return texture;
}


Texture2D* TextureCache::textureForKey(const char* key)
{
	return (Texture2D*)_pTextures->objectForKey(FileUtils::sharedFileUtils()->fullPathForFilename(key));
}


bool TextureCache::reloadTexture(const char* fileName)
{
	std::string fullpath = FileUtils::sharedFileUtils()->fullPathForFilename(fileName);
	if (fullpath.size() == 0)
	{
		return false;
	}

	Texture2D* texture = (Texture2D*)_pTextures->objectForKey(fullpath);

	bool ret = false;
	if (!texture) {
		texture = this->addImage(fullpath.c_str());
		ret = (texture != NULL);
	}
	else
	{
		do {
			Image* image = new Image();
			CC_BREAK_IF(NULL == image);

			bool bRet = image->initWithImageFile(fullpath.c_str());
			CC_BREAK_IF(!bRet);

			ret = texture->initWithImage(image);
		} while (0);
	}

	return ret;
}


void TextureCache::removeTexture(Texture2D* texture)
{
	if (!texture)
	{
		return;
	}

	Array* keys = _pTextures->allKeysForObject(texture);
	_pTextures->removeObjectsForKeys(keys);
}

void TextureCache::removeTextureForKey(const char* textureKeyName)
{
	if (textureKeyName == NULL)
		return;

	std::string fullPath = FileUtils::sharedFileUtils()->fullPathForFilename(textureKeyName);
	_pTextures->removeObjectForKey(fullPath);
}


void TextureCache::removeUnusedTextures()
{
	if (_pTextures->count() == 0)
		return;

	DictElement* pElement = NULL;
	std::list<DictElement*> elementToRemove;
	DICT_FOREACH(_pTextures, pElement)
	{
		CCLOG("cocos2d: CCTextureCache: texture: %s", pElement->getStrKey());
		Texture2D* value = (Texture2D*)pElement->getObject();
		if (value->retainCount() == 1)
		{
			elementToRemove.push_back(pElement);
		}
	}

	for (std::list<DictElement*>::iterator iter = elementToRemove.begin(); iter != elementToRemove.end(); ++iter)
	{
		CCLOG("cocos2d: CCTextureCache: removing unused texture: %s", (*iter)->getStrKey());
		_pTextures->removeObjectForElememt(*iter);
	}
}

void TextureCache::removeAllTextures()
{
	_pTextures->removeAllObjects();
}


void TextureCache::dumpCachedTextureInfo()
{
	unsigned int count = 0;
	unsigned int totalBytes = 0;

	DictElement* pElement = NULL;
	DICT_FOREACH(_pTextures, pElement)
	{
		Texture2D* tex = (Texture2D*)pElement->getObject();
		unsigned int bpp = tex->bitsPerPixelForFormat();

		// Each texture takes up width * height * bytesPerPixel bytes.
		unsigned int bytes = tex->getPixelsWide() * tex->getPixelsHigh() * bpp / 8;
		totalBytes += bytes;
		count++;
		CCLOG("cocos2d: \"%s\" rc=%lu id=%lu %lu x %lu @ %ld bpp => %lu KB",
			pElement->getStrKey(),
			(long)tex->retainCount(),
			(long)tex->getName(),
			(long)tex->getPixelsWide(),
			(long)tex->getPixelsHigh(),
			(long)bpp,
			(long)bytes / 1024);
	}

	CCLOG("cocos2d: CCTextureCache dumpDebugInfo: %ld textures, for %lu KB (%.2f MB)", (long)count, (long)totalBytes / 1024, totalBytes / (1024.0f * 1024.0f));
}



NS_CC_END
