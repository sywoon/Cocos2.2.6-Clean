#include "Texture2D.h"
#include "cocoa/String.h"
#include "shaders/GLProgram.h"
#include "shaders/GLStateCache.h"
#include "shaders/ShaderCache.h"
#include "platform/Image.h"
#include "Configuration.h"
#include "TextureEtc.h"
#include "TexturePvr.h"


NS_CC_BEGIN

static Texture2DPixelFormat g_defaultAlphaPixelFormat = kCCTexture2DPixelFormat_Default;

// By default PVR images are treated as if they don't have the alpha channel premultiplied
static bool PVRHaveAlphaPremultiplied_ = false;



Texture2D::Texture2D()
	: _bHasPremultipliedAlpha(false)
	, _bPVRHaveAlphaPremultiplied(true)
	, _bHasMipmaps(false)

	, _uName(0)
	, _uPixelsWide(0)
	, _uPixelsHigh(0)
	, _fMaxS(0)
	, _fMaxT(0)

	, _pShaderProgram(NULL)
{
}


Texture2D::~Texture2D()
{
	CC_SAFE_RELEASE(_pShaderProgram);

	if (_uName)
	{
		ccGLDeleteTexture(_uName);
		_uName = 0;
	}
}

void Texture2D::setDefaultAlphaPixelFormat(Texture2DPixelFormat format)
{
	g_defaultAlphaPixelFormat = format;
}

Texture2DPixelFormat Texture2D::defaultAlphaPixelFormat()
{
	return g_defaultAlphaPixelFormat;
}


const char* Texture2D::description(void)
{
	return String::createWithFormat(
			"<CCTexture2D | Name = %u | Dimensions = %u x %u | Coordinates = (%.2f, %.2f)>", 
			_uName, _uPixelsWide, _uPixelsHigh, _fMaxS, _fMaxT)->getCString();
}

bool Texture2D::initWithImage(Image* uiImage)
{
	if (uiImage == NULL)
	{
		CCLOG("cocos2d: CCTexture2D. Can't create Texture. UIImage is nil");
		return false;
	}

	unsigned int imageWidth = uiImage->getWidth();
	unsigned int imageHeight = uiImage->getHeight();

	Configuration* conf = Configuration::sharedConfiguration();
	unsigned maxTextureSize = conf->getMaxTextureSize();
	if (imageWidth > maxTextureSize || imageHeight > maxTextureSize)
	{
		CCLOG("cocos2d: WARNING: Image (%u x %u) is bigger than the supported %u x %u", imageWidth, imageHeight, maxTextureSize, maxTextureSize);
		return false;
	}

	// always load premultiplied images
	return initPremultipliedATextureWithImage(uiImage, imageWidth, imageHeight);
}


bool Texture2D::initPremultipliedATextureWithImage(Image* image, unsigned int width, unsigned int height)
{
	unsigned char* tempData = image->getData();
	unsigned int* inPixel32 = NULL;
	unsigned char* inPixel8 = NULL;
	unsigned short* outPixel16 = NULL;
	bool hasAlpha = image->hasAlpha();
	Size imageSize = SizeMake((float)(image->getWidth()), (float)(image->getHeight()));
	Texture2DPixelFormat pixelFormat;
	size_t bpp = image->getBitsPerComponent();

	// compute pixel format
	if (hasAlpha)
	{
		pixelFormat = g_defaultAlphaPixelFormat;
	}
	else
	{
		if (bpp >= 8)
		{
			pixelFormat = kCCTexture2DPixelFormat_RGB888;
		}
		else
		{
			pixelFormat = kCCTexture2DPixelFormat_RGB565;
		}

	}

	// Repack the pixel data into the right format
	unsigned int length = width * height;

	if (pixelFormat == kCCTexture2DPixelFormat_RGB565)
	{
		if (hasAlpha)
		{
			// Convert "RRRRRRRRRGGGGGGGGBBBBBBBBAAAAAAAA" to "RRRRRGGGGGGBBBBB"

			tempData = new unsigned char[width * height * 2];
			outPixel16 = (unsigned short*)tempData;
			inPixel32 = (unsigned int*)image->getData();

			for (unsigned int i = 0; i < length; ++i, ++inPixel32)
			{
				*outPixel16++ =
					((((*inPixel32 >> 0) & 0xFF) >> 3) << 11) |  // R
					((((*inPixel32 >> 8) & 0xFF) >> 2) << 5) |  // G
					((((*inPixel32 >> 16) & 0xFF) >> 3) << 0);    // B
			}
		}
		else
		{
			// Convert "RRRRRRRRRGGGGGGGGBBBBBBBB" to "RRRRRGGGGGGBBBBB"

			tempData = new unsigned char[width * height * 2];
			outPixel16 = (unsigned short*)tempData;
			inPixel8 = (unsigned char*)image->getData();

			for (unsigned int i = 0; i < length; ++i)
			{
				*outPixel16++ =
					(((*inPixel8++ & 0xFF) >> 3) << 11) |  // R
					(((*inPixel8++ & 0xFF) >> 2) << 5) |  // G
					(((*inPixel8++ & 0xFF) >> 3) << 0);    // B
			}
		}
	}
	else if (pixelFormat == kCCTexture2DPixelFormat_RGBA4444)
	{
		// Convert "RRRRRRRRRGGGGGGGGBBBBBBBBAAAAAAAA" to "RRRRGGGGBBBBAAAA"

		inPixel32 = (unsigned int*)image->getData();
		tempData = new unsigned char[width * height * 2];
		outPixel16 = (unsigned short*)tempData;

		for (unsigned int i = 0; i < length; ++i, ++inPixel32)
		{
			*outPixel16++ =
				((((*inPixel32 >> 0) & 0xFF) >> 4) << 12) | // R
				((((*inPixel32 >> 8) & 0xFF) >> 4) << 8) | // G
				((((*inPixel32 >> 16) & 0xFF) >> 4) << 4) | // B
				((((*inPixel32 >> 24) & 0xFF) >> 4) << 0);  // A
		}
	}
	else if (pixelFormat == kCCTexture2DPixelFormat_RGB5A1)
	{
		// Convert "RRRRRRRRRGGGGGGGGBBBBBBBBAAAAAAAA" to "RRRRRGGGGGBBBBBA"
		inPixel32 = (unsigned int*)image->getData();
		tempData = new unsigned char[width * height * 2];
		outPixel16 = (unsigned short*)tempData;

		for (unsigned int i = 0; i < length; ++i, ++inPixel32)
		{
			*outPixel16++ =
				((((*inPixel32 >> 0) & 0xFF) >> 3) << 11) | // R
				((((*inPixel32 >> 8) & 0xFF) >> 3) << 6) | // G
				((((*inPixel32 >> 16) & 0xFF) >> 3) << 1) | // B
				((((*inPixel32 >> 24) & 0xFF) >> 7) << 0);  // A
		}
	}
	else if (pixelFormat == kCCTexture2DPixelFormat_A8)
	{
		// Convert "RRRRRRRRRGGGGGGGGBBBBBBBBAAAAAAAA" to "AAAAAAAA"
		inPixel32 = (unsigned int*)image->getData();
		tempData = new unsigned char[width * height];
		unsigned char* outPixel8 = tempData;

		for (unsigned int i = 0; i < length; ++i, ++inPixel32)
		{
			*outPixel8++ = (*inPixel32 >> 24) & 0xFF;  // A
		}
	}

	if (hasAlpha && pixelFormat == kCCTexture2DPixelFormat_RGB888)
	{
		// Convert "RRRRRRRRRGGGGGGGGBBBBBBBBAAAAAAAA" to "RRRRRRRRGGGGGGGGBBBBBBBB"
		inPixel32 = (unsigned int*)image->getData();
		tempData = new unsigned char[width * height * 3];
		unsigned char* outPixel8 = tempData;

		for (unsigned int i = 0; i < length; ++i, ++inPixel32)
		{
			*outPixel8++ = (*inPixel32 >> 0) & 0xFF; // R
			*outPixel8++ = (*inPixel32 >> 8) & 0xFF; // G
			*outPixel8++ = (*inPixel32 >> 16) & 0xFF; // B
		}
	}

	initWithData(tempData, pixelFormat, width, height, imageSize);

	if (tempData != image->getData())
	{
		delete[] tempData;
	}

	_bHasPremultipliedAlpha = image->isPremultipliedAlpha();
	return true;
}


bool Texture2D::initWithData(const void* data, Texture2DPixelFormat pixelFormat, 
				unsigned int pixelsWide, unsigned int pixelsHigh, const Size& contentSize)
{
	unsigned int bitsPerPixel;
	//Hack: bitsPerPixelForFormat returns wrong number for RGB_888 textures. See function.
	if (pixelFormat == kCCTexture2DPixelFormat_RGB888)
	{
		bitsPerPixel = 24;
	}
	else
	{
		bitsPerPixel = bitsPerPixelForFormat(pixelFormat);
	}

	unsigned int bytesPerRow = pixelsWide * bitsPerPixel / 8;

	if (bytesPerRow % 8 == 0)
	{
		glPixelStorei(GL_UNPACK_ALIGNMENT, 8);
	}
	else if (bytesPerRow % 4 == 0)
	{
		glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
	}
	else if (bytesPerRow % 2 == 0)
	{
		glPixelStorei(GL_UNPACK_ALIGNMENT, 2);
	}
	else
	{
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	}

	glGenTextures(1, &_uName);
	ccGLBindTexture2D(_uName);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	// Specify OpenGL texture image
	switch (pixelFormat)
	{
	case kCCTexture2DPixelFormat_RGBA8888:
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, (GLsizei)pixelsWide, (GLsizei)pixelsHigh, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		break;
	case kCCTexture2DPixelFormat_RGB888:
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, (GLsizei)pixelsWide, (GLsizei)pixelsHigh, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		break;
	case kCCTexture2DPixelFormat_RGBA4444:
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, (GLsizei)pixelsWide, (GLsizei)pixelsHigh, 0, GL_RGBA, GL_UNSIGNED_SHORT_4_4_4_4, data);
		break;
	case kCCTexture2DPixelFormat_RGB5A1:
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, (GLsizei)pixelsWide, (GLsizei)pixelsHigh, 0, GL_RGBA, GL_UNSIGNED_SHORT_5_5_5_1, data);
		break;
	case kCCTexture2DPixelFormat_RGB565:
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, (GLsizei)pixelsWide, (GLsizei)pixelsHigh, 0, GL_RGB, GL_UNSIGNED_SHORT_5_6_5, data);
		break;
	case kCCTexture2DPixelFormat_AI88:
		glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE_ALPHA, (GLsizei)pixelsWide, (GLsizei)pixelsHigh, 0, GL_LUMINANCE_ALPHA, GL_UNSIGNED_BYTE, data);
		break;
	case kCCTexture2DPixelFormat_A8:
		glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, (GLsizei)pixelsWide, (GLsizei)pixelsHigh, 0, GL_ALPHA, GL_UNSIGNED_BYTE, data);
		break;
	case kCCTexture2DPixelFormat_I8:
		glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, (GLsizei)pixelsWide, (GLsizei)pixelsHigh, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, data);
		break;
	default:
		CCAssert(0, "NSInternalInconsistencyException");

	}

	_tContentSize = contentSize;
	_uPixelsWide = pixelsWide;
	_uPixelsHigh = pixelsHigh;
	_ePixelFormat = pixelFormat;
	_fMaxS = contentSize.width / (float)(pixelsWide);
	_fMaxT = contentSize.height / (float)(pixelsHigh);

	_bHasPremultipliedAlpha = false;
	_bHasMipmaps = false;

	setShaderProgram(ShaderCache::sharedShaderCache()->programForKey(kCCShader_PositionTexture));

	return true;
}


bool Texture2D::initWithString(const char* text, const char* fontName, float fontSize, 
					const Size& dimensions, CCTextAlignment hAlignment,
					CCVerticalTextAlignment vAlignment)
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID) || (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
	ccFontDefinition tempDef;

	tempDef.m_shadow.m_shadowEnabled = false;
	tempDef.m_stroke.m_strokeEnabled = false;

	tempDef.m_fontName = std::string(fontName);
	tempDef.m_fontSize = fontSize;
	tempDef.m_dimensions = dimensions;
	tempDef.m_alignment = hAlignment;
	tempDef.m_vertAlignment = vAlignment;
	tempDef.m_fontFillColor = ccWHITE;

	return initWithString(text, &tempDef);
#else

	bool bRet = false;
	Image::ETextAlign eAlign;

	if (kCCVerticalTextAlignmentTop == vAlignment)
	{
		eAlign = (kCCTextAlignmentCenter == hAlignment) ? Image::kAlignTop
			: (kCCTextAlignmentLeft == hAlignment) ? Image::kAlignTopLeft : Image::kAlignTopRight;
	}
	else if (kCCVerticalTextAlignmentCenter == vAlignment)
	{
		eAlign = (kCCTextAlignmentCenter == hAlignment) ? Image::kAlignCenter
			: (kCCTextAlignmentLeft == hAlignment) ? Image::kAlignLeft : Image::kAlignRight;
	}
	else if (kCCVerticalTextAlignmentBottom == vAlignment)
	{
		eAlign = (kCCTextAlignmentCenter == hAlignment) ? Image::kAlignBottom
			: (kCCTextAlignmentLeft == hAlignment) ? Image::kAlignBottomLeft : Image::kAlignBottomRight;
	}
	else
	{
		CCAssert(false, "Not supported alignment format!");
		return false;
	}

	do
	{
		Image* pImage = new Image();
		CC_BREAK_IF(NULL == pImage);
		bRet = pImage->initWithString(text, (int)dimensions.width, (int)dimensions.height, eAlign, fontName, (int)fontSize);
		CC_BREAK_IF(!bRet);
		bRet = initWithImage(pImage);
		CC_SAFE_RELEASE(pImage);
	} while (0);


	return bRet;
#endif

}

bool Texture2D::initWithString(const char* text, const char* fontName, float fontSize)
{
	return initWithString(text, fontName, fontSize, SizeMake(0, 0), kCCTextAlignmentCenter, kCCVerticalTextAlignmentTop);
}


bool Texture2D::initWithString(const char* text, ccFontDefinition* textDefinition)
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID) || (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)

	bool bRet = false;
	Image::ETextAlign eAlign;

	if (kCCVerticalTextAlignmentTop == textDefinition->m_vertAlignment)
	{
		eAlign = (kCCTextAlignmentCenter == textDefinition->m_alignment) ? Image::kAlignTop
			: (kCCTextAlignmentLeft == textDefinition->m_alignment) ? Image::kAlignTopLeft : Image::kAlignTopRight;
	}
	else if (kCCVerticalTextAlignmentCenter == textDefinition->m_vertAlignment)
	{
		eAlign = (kCCTextAlignmentCenter == textDefinition->m_alignment) ? Image::kAlignCenter
			: (kCCTextAlignmentLeft == textDefinition->m_alignment) ? Image::kAlignLeft : Image::kAlignRight;
	}
	else if (kCCVerticalTextAlignmentBottom == textDefinition->m_vertAlignment)
	{
		eAlign = (kCCTextAlignmentCenter == textDefinition->m_alignment) ? Image::kAlignBottom
			: (kCCTextAlignmentLeft == textDefinition->m_alignment) ? Image::kAlignBottomLeft : Image::kAlignBottomRight;
	}
	else
	{
		CCAssert(false, "Not supported alignment format!");
		return false;
	}

	// handle shadow parameters
	bool  shadowEnabled = false;
	float shadowDX = 0.0f;
	float shadowDY = 0.0f;
	float shadowBlur = 0.0f;
	float shadowOpacity = 0.0f;

	if (textDefinition->m_shadow.m_shadowEnabled)
	{
		shadowEnabled = true;
		shadowDX = textDefinition->m_shadow.m_shadowOffset.width;
		shadowDY = textDefinition->m_shadow.m_shadowOffset.height;
		shadowBlur = textDefinition->m_shadow.m_shadowBlur;
		shadowOpacity = textDefinition->m_shadow.m_shadowOpacity;
	}

	// handle stroke parameters
	bool strokeEnabled = false;
	float strokeColorR = 0.0f;
	float strokeColorG = 0.0f;
	float strokeColorB = 0.0f;
	float strokeSize = 0.0f;

	if (textDefinition->m_stroke.m_strokeEnabled)
	{
		strokeEnabled = true;
		strokeColorR = textDefinition->m_stroke.m_strokeColor.r / 255.0f;
		strokeColorG = textDefinition->m_stroke.m_strokeColor.g / 255.0f;
		strokeColorB = textDefinition->m_stroke.m_strokeColor.b / 255.0f;
		strokeSize = textDefinition->m_stroke.m_strokeSize;
	}

	Image* pImage = new Image();
	do
	{
		CC_BREAK_IF(NULL == pImage);

		bRet = pImage->initWithStringShadowStroke(text,
			(int)textDefinition->m_dimensions.width,
			(int)textDefinition->m_dimensions.height,
			eAlign,
			textDefinition->m_fontName.c_str(),
			textDefinition->m_fontSize,
			textDefinition->m_fontFillColor.r / 255.0f,
			textDefinition->m_fontFillColor.g / 255.0f,
			textDefinition->m_fontFillColor.b / 255.0f,
			shadowEnabled,
			shadowDX,
			shadowDY,
			shadowOpacity,
			shadowBlur,
			strokeEnabled,
			strokeColorR,
			strokeColorG,
			strokeColorB,
			strokeSize);


		CC_BREAK_IF(!bRet);
		bRet = initWithImage(pImage);

	} while (0);

	CC_SAFE_RELEASE(pImage);

	return bRet;


#else

	CCAssert(false, "Currently only supported on iOS and Android!");
	return false;

#endif
}



void Texture2D::PVRImagesHavePremultipliedAlpha(bool haveAlphaPremultiplied)
{
	PVRHaveAlphaPremultiplied_ = haveAlphaPremultiplied;
}

bool Texture2D::initWithPVRFile(const char* file)
{
	bool bRet = false;
	// nothing to do with CCObject::init

	TexturePVR* pvr = new TexturePVR();
	bRet = pvr->initWithContentsOfFile(file);

	if (bRet)
	{
		pvr->setRetainName(true); // don't dealloc texture on release

		_uName = pvr->getName();
		_fMaxS = 1.0f;
		_fMaxT = 1.0f;
		_uPixelsWide = pvr->getWidth();
		_uPixelsHigh = pvr->getHeight();
		_tContentSize = SizeMake((float)_uPixelsWide, (float)_uPixelsHigh);
		_bHasPremultipliedAlpha = PVRHaveAlphaPremultiplied_;
		_ePixelFormat = pvr->getFormat();
		_bHasMipmaps = pvr->getNumberOfMipmaps() > 1;

		pvr->release();
	}
	else
	{
		CCLOG("cocos2d: Couldn't load PVR image %s", file);
	}

	return bRet;
}


bool Texture2D::initWithETCFile(const char* file)
{
	bool bRet = false;
	// nothing to do with CCObject::init

	TextureETC* etc = new TextureETC;
	bRet = etc->initWithFile(file);

	if (bRet)
	{
		_uName = etc->getName();
		_fMaxS = 1.0f;
		_fMaxT = 1.0f;
		_uPixelsWide = etc->getWidth();
		_uPixelsHigh = etc->getHeight();
		_tContentSize = SizeMake((float)_uPixelsWide, (float)_uPixelsHigh);
		_bHasPremultipliedAlpha = true;

		etc->release();
	}
	else
	{
		CCLOG("cocos2d: Couldn't load ETC image %s", file);
	}

	return bRet;
}












unsigned int Texture2D::bitsPerPixelForFormat(Texture2DPixelFormat format)
{
	unsigned int ret = 0;

	switch (format) {
	case kCCTexture2DPixelFormat_RGBA8888:
		ret = 32;
		break;
	case kCCTexture2DPixelFormat_RGB888:
		// It is 32 and not 24, since its internal representation uses 32 bits.
		ret = 32;  //内存中实际大小
		break;
	case kCCTexture2DPixelFormat_RGB565:
		ret = 16;
		break;
	case kCCTexture2DPixelFormat_RGBA4444:
		ret = 16;
		break;
	case kCCTexture2DPixelFormat_RGB5A1:
		ret = 16;
		break;
	case kCCTexture2DPixelFormat_AI88:
		ret = 16;
		break;
	case kCCTexture2DPixelFormat_A8:
		ret = 8;
		break;
	case kCCTexture2DPixelFormat_I8:
		ret = 8;
		break;
	case kCCTexture2DPixelFormat_PVRTC4:
		ret = 4;
		break;
	case kCCTexture2DPixelFormat_PVRTC2:
		ret = 2;
		break;
	default:
		ret = -1;
		CCAssert(false, "unrecognized pixel format");
		CCLOG("bitsPerPixelForFormat: %ld, cannot give useful result", (long)format);
		break;
	}
	return ret;
}

unsigned int Texture2D::bitsPerPixelForFormat()
{
	return this->bitsPerPixelForFormat(_ePixelFormat);
}

bool Texture2D::hasPremultipliedAlpha()
{
	return _bHasPremultipliedAlpha;
}

bool Texture2D::hasMipmaps()
{
	return _bHasMipmaps;
}

const Size& Texture2D::getContentSizeInPixels()
{
	return _tContentSize;
}



Texture2DPixelFormat Texture2D::getPixelFormat()
{
	return _ePixelFormat;
}

unsigned int Texture2D::getPixelsWide()
{
	return _uPixelsWide;
}

unsigned int Texture2D::getPixelsHigh()
{
	return _uPixelsHigh;
}

GLuint Texture2D::getName()
{
	return _uName;
}

Size Texture2D::getContentSize()
{

	Size ret;
	ret.width = _tContentSize.width;
	ret.height = _tContentSize.height;

	return ret;
}

GLfloat Texture2D::getMaxS()
{
	return _fMaxS;
}

void Texture2D::setMaxS(GLfloat maxS)
{
	_fMaxS = maxS;
}

GLfloat Texture2D::getMaxT()
{
	return _fMaxT;
}

void Texture2D::setMaxT(GLfloat maxT)
{
	_fMaxT = maxT;
}

GLProgram* Texture2D::getShaderProgram(void)
{
	return _pShaderProgram;
}

void Texture2D::setShaderProgram(GLProgram* pShaderProgram)
{
	CC_SAFE_RETAIN(pShaderProgram);
	CC_SAFE_RELEASE(_pShaderProgram);
	_pShaderProgram = pShaderProgram;
}


void Texture2D::drawAtPoint(const Point& point)
{
	GLfloat coordinates[] = {
		0.0f,    _fMaxT,
		_fMaxS,_fMaxT,
		0.0f,    0.0f,
		_fMaxS,0.0f };

	GLfloat width = (GLfloat)_uPixelsWide * _fMaxS;
	GLfloat height = (GLfloat)_uPixelsHigh * _fMaxT;

	GLfloat vertices[] = {
		point.x,            point.y,
		width + point.x,    point.y,
		point.x,            height + point.y,
		width + point.x,    height + point.y };

	ccGLEnableVertexAttribs(kCCVertexAttribFlag_Position | kCCVertexAttribFlag_TexCoords);
	_pShaderProgram->use();
	_pShaderProgram->setUniformsForBuiltins();

	ccGLBindTexture2D(_uName);


#ifdef EMSCRIPTEN
	setGLBufferData(vertices, 8 * sizeof(GLfloat), 0);
	glVertexAttribPointer(kCCVertexAttrib_Position, 2, GL_FLOAT, GL_FALSE, 0, 0);

	setGLBufferData(coordinates, 8 * sizeof(GLfloat), 1);
	glVertexAttribPointer(kCCVertexAttrib_TexCoords, 2, GL_FLOAT, GL_FALSE, 0, 0);
#else
	glVertexAttribPointer(kCCVertexAttrib_Position, 2, GL_FLOAT, GL_FALSE, 0, vertices);
	glVertexAttribPointer(kCCVertexAttrib_TexCoords, 2, GL_FLOAT, GL_FALSE, 0, coordinates);
#endif // EMSCRIPTEN

	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}


void Texture2D::drawInRect(const Rect& rect)
{
	GLfloat    coordinates[] = {
		0.0f,    _fMaxT,
		_fMaxS,_fMaxT,
		0.0f,    0.0f,
		_fMaxS,0.0f };

	GLfloat    vertices[] = { rect.origin.x,        rect.origin.y,                            /*0.0f,*/
		rect.origin.x + rect.size.width,        rect.origin.y,                            /*0.0f,*/
		rect.origin.x,                            rect.origin.y + rect.size.height,        /*0.0f,*/
		rect.origin.x + rect.size.width,        rect.origin.y + rect.size.height,        /*0.0f*/ };

	ccGLEnableVertexAttribs(kCCVertexAttribFlag_Position | kCCVertexAttribFlag_TexCoords);
	_pShaderProgram->use();
	_pShaderProgram->setUniformsForBuiltins();

	ccGLBindTexture2D(_uName);

#ifdef EMSCRIPTEN
	setGLBufferData(vertices, 8 * sizeof(GLfloat), 0);
	glVertexAttribPointer(kCCVertexAttrib_Position, 2, GL_FLOAT, GL_FALSE, 0, 0);

	setGLBufferData(coordinates, 8 * sizeof(GLfloat), 1);
	glVertexAttribPointer(kCCVertexAttrib_TexCoords, 2, GL_FLOAT, GL_FALSE, 0, 0);
#else
	glVertexAttribPointer(kCCVertexAttrib_Position, 2, GL_FLOAT, GL_FALSE, 0, vertices);
	glVertexAttribPointer(kCCVertexAttrib_TexCoords, 2, GL_FLOAT, GL_FALSE, 0, coordinates);
#endif // EMSCRIPTEN
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}



NS_CC_END