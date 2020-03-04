#pragma once

#include "ccMacros.h"
#include "platform/Image.h"


NS_CC_BEGIN

//////////////////////////////////
// Image�ӿڵ�ƽ̨ʵ�֣����cpp��
//


//////////////////////////////////
// Win32 DC�ķ�װ  
//
class BitmapDC
{
	friend class CCImage;

public:
	BitmapDC(HWND hWnd = NULL);
	~BitmapDC();

	static BitmapDC& sharedBitmapDC();

public:
	bool prepareBitmap(int nWidth, int nHeight);
	bool setFont(const char* pFontName = NULL, int nSize = 0);
	SIZE sizeWithText(const char* pszText, int nLen, DWORD dwFmt, LONG nWidthLimit);
	int drawText(const char* pszText, SIZE& tSize, CCImage::ETextAlign eAlign);

private:
	CC_SYNTHESIZE_READONLY(HDC, m_hDC, DC);
	CC_SYNTHESIZE_READONLY(HBITMAP, m_hBmp, Bitmap);

	HWND    m_hWnd;
	HFONT   m_hFont;
	std::string m_curFontPath;
};



NS_CC_END