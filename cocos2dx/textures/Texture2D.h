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

} CCTexture2DPixelFormat;



class GLProgram;
class Image;
class CC_DLL CCTexture2D : public Object
{
public:
    CCTexture2D();
    virtual ~CCTexture2D();

public:
	static void setDefaultAlphaPixelFormat(CCTexture2DPixelFormat format);
	static CCTexture2DPixelFormat defaultAlphaPixelFormat();

public:
    const char* description(void);

	bool initWithImage(Image* uiImage);
	bool initWithString(const char* text, const char* fontName, float fontSize, 
				const Size& dimensions, CCTextAlignment hAlignment, 
				CCVerticalTextAlignment vAlignment);
	bool initWithString(const char* text, const char* fontName, float fontSize);
	bool initWithString(const char* text, ccFontDefinition* textDefinition);

	bool initWithData(const void* data, CCTexture2DPixelFormat pixelFormat, 
				unsigned int pixelsWide, unsigned int pixelsHigh, const Size& contentSize);


	bool initWithPVRFile(const char* file);
	bool initWithETCFile(const char* file);

private:
	bool initPremultipliedATextureWithImage(Image* image, unsigned int pixelsWide, unsigned int pixelsHigh);

private:
	unsigned int bitsPerPixelForFormat();
	unsigned int bitsPerPixelForFormat(CCTexture2DPixelFormat format);


public:
	static void PVRImagesHavePremultipliedAlpha(bool haveAlphaPremultiplied);


public:
	void drawAtPoint(const Point& point);
	void drawInRect(const Rect& rect);


private:
    CC_PROPERTY_READONLY(GLuint, m_uName, Name)
	CC_PROPERTY_READONLY(CCTexture2DPixelFormat, m_ePixelFormat, PixelFormat)

	CC_PROPERTY_READONLY(unsigned int, m_uPixelsWide, PixelsWide)
	CC_PROPERTY_READONLY(unsigned int, m_uPixelsHigh, PixelsHigh)

	CC_PROPERTY(GLfloat, m_fMaxS, MaxS)
	CC_PROPERTY(GLfloat, m_fMaxT, MaxT)

	CC_PROPERTY_READONLY(Size, m_tContentSize, ContentSize)
	CC_PROPERTY(GLProgram*, m_pShaderProgram, ShaderProgram);


private:
	bool	m_bHasPremultipliedAlpha;
	bool	m_bPVRHaveAlphaPremultiplied;
	bool	m_bHasMipmaps;
};





NS_CC_END
