#pragma once

#include "ccMacros.h"
#include "cocoa/Object.h"



NS_CC_BEGIN


class Texture2D;
class Image;

class CC_DLL TextureCache : public Object
{
public:
    TextureCache();
    virtual ~TextureCache();

    static TextureCache*  sharedTextureCache();
    static void           purgeSharedTextureCache();


public:
    const char*     description(void);

    Texture2D*      addImage(const char* fileimage);
    void            addImageAsync(const char* path, Object* target, SEL_CallFuncO selector);

    Texture2D*      addUIImage(Image* image, const char* key);

public:
    Texture2D*      textureForKey(const char* key);
    bool            reloadTexture(const char* fileName);

    void            removeTexture(Texture2D* texture);
    void            removeTextureForKey(const char* textureKeyName);

    void            removeUnusedTextures();
    void            removeAllTextures();

    void            dumpCachedTextureInfo();

protected:
    Texture2D*      addPVRImage(const char* filename);
    Texture2D*      addETCImage(const char* filename);

    void            addImageAsyncCallBack(float dt);


protected:
	Dictionary*     _pTextures;
};




NS_CC_END
