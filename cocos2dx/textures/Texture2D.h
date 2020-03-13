#pragma once

#include "ccMacros.h"
#include "cocoa/Object.h"
#include "cocoa/Geometry.h"



NS_CC_BEGIN

typedef enum {
	//! 32-bit texture: RGBA8888
	kCCTexture2DPixelFormat_RGBA8888,
	//! 24-bit texture: RGBA888
	kCCTexture2DPixelFormat_RGB888,
	//! 16-bit texture without Alpha channel
	kCCTexture2DPixelFormat_RGB565,
	//! 8-bit textures used as masks
	kCCTexture2DPixelFormat_A8,
	//! 8-bit intensity texture
	kCCTexture2DPixelFormat_I8,
	//! 16-bit textures used as masks
	kCCTexture2DPixelFormat_AI88,
	//! 16-bit textures: RGBA4444
	kCCTexture2DPixelFormat_RGBA4444,
	//! 16-bit textures: RGB5A1
	kCCTexture2DPixelFormat_RGB5A1,
	//! 4-bit PVRTC-compressed texture: PVRTC4
	kCCTexture2DPixelFormat_PVRTC4,
	//! 2-bit PVRTC-compressed texture: PVRTC2
	kCCTexture2DPixelFormat_PVRTC2,


	//! Default texture format: RGBA8888
	kCCTexture2DPixelFormat_Default = kCCTexture2DPixelFormat_RGBA8888,

	// backward compatibility stuff
	kTexture2DPixelFormat_RGBA8888 = kCCTexture2DPixelFormat_RGBA8888,
	kTexture2DPixelFormat_RGB888 = kCCTexture2DPixelFormat_RGB888,
	kTexture2DPixelFormat_RGB565 = kCCTexture2DPixelFormat_RGB565,
	kTexture2DPixelFormat_A8 = kCCTexture2DPixelFormat_A8,
	kTexture2DPixelFormat_RGBA4444 = kCCTexture2DPixelFormat_RGBA4444,
	kTexture2DPixelFormat_RGB5A1 = kCCTexture2DPixelFormat_RGB5A1,
	kTexture2DPixelFormat_Default = kCCTexture2DPixelFormat_Default

} Texture2DPixelFormat;



class GLProgram;
class Image;
class CC_DLL Texture2D : public Object
{
public:
    Texture2D();
    virtual ~Texture2D();

public:
	static void setDefaultAlphaPixelFormat(Texture2DPixelFormat format);
	static Texture2DPixelFormat defaultAlphaPixelFormat();

public:
    const char* description(void);

	bool		initWithImage(Image* uiImage);
	bool		initWithString(const char* text, const char* fontName, float fontSize, 
					const Size& dimensions, CCTextAlignment hAlignment, 
					CCVerticalTextAlignment vAlignment);
	bool		initWithString(const char* text, const char* fontName, float fontSize);
	bool		initWithString(const char* text, ccFontDefinition* textDefinition);

	bool		initWithData(const void* data, Texture2DPixelFormat pixelFormat, 
					unsigned int pixelsWide, unsigned int pixelsHigh, const Size& contentSize);

	bool		initWithPVRFile(const char* file);
	bool		initWithETCFile(const char* file);

private:
	bool		initPremultipliedATextureWithImage(Image* image, unsigned int pixelsWide, unsigned int pixelsHigh);


public:
	unsigned int bitsPerPixelForFormat();
	unsigned int bitsPerPixelForFormat(Texture2DPixelFormat format);

	bool		hasPremultipliedAlpha();
	bool		hasMipmaps();

	const Size& getContentSizeInPixels();

public:
	static void PVRImagesHavePremultipliedAlpha(bool haveAlphaPremultiplied);


public:
	void		drawAtPoint(const Point& point);
	void		drawInRect(const Rect& rect);


private:
    CC_PROPERTY_READONLY(GLuint, _uName, Name)
	CC_PROPERTY_READONLY(Texture2DPixelFormat, _ePixelFormat, PixelFormat)

	CC_PROPERTY_READONLY(unsigned int, _uPixelsWide, PixelsWide)
	CC_PROPERTY_READONLY(unsigned int, _uPixelsHigh, PixelsHigh)

	CC_PROPERTY(GLfloat, _fMaxS, MaxS)
	CC_PROPERTY(GLfloat, _fMaxT, MaxT)

	CC_PROPERTY_READONLY(Size, _tContentSize, ContentSize)
	CC_PROPERTY(GLProgram*, _pShaderProgram, ShaderProgram);


private:
	bool	_bHasPremultipliedAlpha;
	bool	_bPVRHaveAlphaPremultiplied;
	bool	_bHasMipmaps;
};





NS_CC_END
