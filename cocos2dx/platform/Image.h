#pragma once

#include "ccMacros.h"
#include "ccClassMacros.h"
#include "PlatformMacros.h"
#include "cocoa/Object.h"



NS_CC_BEGIN



class CC_DLL CCImage : public Object
{
public:
	typedef enum
	{
		kFmtJpg = 0,
		kFmtPng,
		kFmtTiff,
		kFmtWebp,
		kFmtRawData,   //only support RGBA8888
		kFmtUnKnown
	}EImageFormat;

	typedef enum
	{
		kAlignTopLeft = 0x11, ///< Horizontal left and vertical top.
		kAlignTopRight = 0x12, ///< Horizontal right and vertical top.

		kAlignTop = 0x13, ///< Horizontal center and vertical top.
		kAlignLeft = 0x31, ///< Horizontal left and vertical center.
		kAlignRight = 0x32, ///< Horizontal right and vertical center.
		kAlignCenter = 0x33, ///< Horizontal center and vertical center.
		kAlignBottom = 0x23, ///< Horizontal center and vertical bottom.

		kAlignBottomLeft = 0x21, ///< Horizontal left and vertical bottom.
		kAlignBottomRight = 0x22, ///< Horizontal right and vertical bottom.
	}ETextAlign;


public:
    CCImage();
    ~CCImage();


private:
	// noncopyable
	CCImage(const CCImage& rImg);
	CCImage& operator=(const CCImage& rImg);

public:
	bool initWithImageFile(const char* strPath, EImageFormat imageType = kFmtPng);
	bool initWithImageFileThreadSafe(const char* fullpath, EImageFormat imageType = kFmtPng);
	bool initWithImageData(void* pData, int nDataLen, EImageFormat eFmt = kFmtUnKnown,
							int nWidth = 0, int nHeight = 0, int nBitsPerComponent = 8);
	
	// 不同平台实现
	bool initWithString(const char* pText, int nWidth = 0, int nHeight = 0, 
						ETextAlign eAlignMask = kAlignCenter, const char* pFontName = 0, int nSize = 0);

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID) || (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
	bool initWithStringShadowStroke(
		const char* pText,
		int             nWidth = 0,
		int             nHeight = 0,
		ETextAlign      eAlignMask = kAlignCenter,
		const char* pFontName = 0,
		int             nSize = 0,
		float           textTintR = 1,
		float           textTintG = 1,
		float           textTintB = 1,
		bool shadow = false,
		float shadowOffsetX = 0.0,
		float shadowOffsetY = 0.0,
		float shadowOpacity = 0.0,
		float shadowBlur = 0.0,
		bool  stroke = false,
		float strokeR = 1,
		float strokeG = 1,
		float strokeB = 1,
		float strokeSize = 1
	);
#endif


protected:
	bool _initWithPngData(void* pData, int nDatalen);
	bool _initWithJpgData(void* pData, int nDatalen);
	bool _initWithTiffData(void* pData, int nDataLen);
	bool _initWithWebpData(void* pData, int nDataLen);
	bool _initWithRawData(void* pData, int nDatalen, int nWidth, int nHeight, int nBitsPerComponent, bool bPreMulti);


public:
	unsigned char*  getData() { return m_pData; }
	int             getDataLen() { return m_nWidth * m_nHeight; }
	bool			hasAlpha() { return m_bHasAlpha; }
	bool			isPremultipliedAlpha() { return m_bPreMulti; }

public:
	bool saveToFile(const char* pszFilePath, bool bIsToRGB = true);

protected:
	bool _saveImageToPNG(const char* pszFilePath, bool bIsToRGB = true);
	bool _saveImageToJPG(const char* pszFilePath);

protected:
	CC_SYNTHESIZE_READONLY(unsigned short, m_nWidth, Width);
	CC_SYNTHESIZE_READONLY(unsigned short, m_nHeight, Height);
	CC_SYNTHESIZE_READONLY(int, m_nBitsPerComponent, BitsPerComponent);

protected:
	unsigned char*	m_pData;
	bool			m_bHasAlpha;
	bool			m_bPreMulti;
};



NS_CC_END