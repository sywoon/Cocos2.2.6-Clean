#pragma once

#include "ccMacros.h"
#include "textures/Texture2D.h"


NS_CC_BEGIN

struct CCPVRMipmap {
	unsigned char* address;
	unsigned int len;
};

enum {
	CC_PVRMIPMAP_MAX = 16,
};

typedef struct _ccPVRTexturePixelFormatInfo {
	GLenum internalFormat;
	GLenum format;
	GLenum type;
	uint32_t bpp;
	bool compressed;
	bool alpha;

	CCTexture2DPixelFormat ccPixelFormat;
} ccPVRTexturePixelFormatInfo;


class TexturePVR : public Object
{
public:
	TexturePVR();
	virtual ~TexturePVR();
	static TexturePVR* create(const char* path);


public:
	bool				initWithContentsOfFile(const char* path);

	inline unsigned int getName() { return m_uName; }
	inline unsigned int getWidth() { return m_uWidth; }
	inline unsigned int getHeight() { return m_uHeight; }

	inline bool			hasAlpha() { return m_bHasAlpha; }
	inline bool			hasPremultipliedAlpha() { return m_bHasPremultipliedAlpha; }
	inline bool			isForcePremultipliedAlpha() { return m_bForcePremultipliedAlpha; }
	inline unsigned int getNumberOfMipmaps() { return m_uNumberOfMipmaps; }
	inline CCTexture2DPixelFormat getFormat() { return m_eFormat; }
	inline bool			isRetainName() { return m_bRetainName; }
	inline void			setRetainName(bool retainName) { m_bRetainName = retainName; }


private:
	bool unpackPVRv2Data(unsigned char* data, unsigned int len);
	bool unpackPVRv3Data(unsigned char* dataPointer, unsigned int dataLength);
	bool createGLTexture();


protected:
	struct CCPVRMipmap m_asMipmaps[CC_PVRMIPMAP_MAX];
	unsigned int	m_uNumberOfMipmaps;

	unsigned int	m_uWidth, m_uHeight;
	GLuint			m_uName;
	bool			m_bHasAlpha;
	bool			m_bHasPremultipliedAlpha;
	bool			m_bForcePremultipliedAlpha;

	bool			m_bRetainName;
	CCTexture2DPixelFormat m_eFormat;

	const ccPVRTexturePixelFormatInfo* m_pPixelFormatInfo;
};






NS_CC_END