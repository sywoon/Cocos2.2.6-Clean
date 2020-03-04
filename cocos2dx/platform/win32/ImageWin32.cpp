#include <windows.h>
#include "ImageWin32.h"
#include "CommonWin32.h"


NS_CC_BEGIN


//////////////////////////////////
// Win32 DC的封装  
//
static BitmapDC& sharedBitmapDC()
{
	static BitmapDC s_BmpDC;
	return s_BmpDC;
}


BitmapDC::BitmapDC(HWND hWnd /*= NULL*/)
	: m_hWnd(hWnd)
	, m_hBmp(NULL)
{
	HDC hdc = GetDC(hWnd);
	m_hDC = CreateCompatibleDC(hdc);
	ReleaseDC(hWnd, hdc);

	m_hFont = ((HFONT)GetStockObject(DEFAULT_GUI_FONT));
}

BitmapDC::~BitmapDC()
{
	prepareBitmap(0, 0);
	if (m_hDC)
	{
		DeleteDC(m_hDC);
	}

	HFONT hDefFont = (HFONT)GetStockObject(DEFAULT_GUI_FONT);
	if (hDefFont != m_hFont)
	{
		DeleteObject(m_hFont);
		m_hFont = hDefFont;
	}

	if (m_curFontPath.size() > 0)
	{
		std::string fontPath = utf8Togbk(m_curFontPath.c_str());
		if (fontPath.length() > 0)
		{
			RemoveFontResource(fontPath.c_str());
			SendMessage(m_hWnd, WM_FONTCHANGE, 0, 0);
		}
	}
}


bool BitmapDC::prepareBitmap(int nWidth, int nHeight)
{
	if (m_hBmp)
	{
		DeleteObject(m_hBmp);
		m_hBmp = NULL;
	}
	if (nWidth > 0 && nHeight > 0)
	{
		m_hBmp = CreateBitmap(nWidth, nHeight, 1, 32, NULL);
		if (!m_hBmp)
		{
			return false;
		}
	}
	return true;
}

bool BitmapDC::setFont(const char* pFontName/* = NULL*/, int nSize/* = 0*/)
{
	if (pFontName == NULL || pFontName[0] == '\0')
		return false;

	bool bRet = false;
	do
	{
		std::string fontName = pFontName;
		std::string fontPath;
		HFONT       hDefFont = (HFONT)GetStockObject(DEFAULT_GUI_FONT);
		LOGFONTA    tNewFont = { 0 };
		LOGFONTA    tOldFont = { 0 };
		GetObjectA(hDefFont, sizeof(tNewFont), &tNewFont);

		{ // create font from ttf file
			int nFindttf = fontName.find(".ttf");
			int nFindTTF = fontName.find(".TTF");
			if (nFindttf >= 0 || nFindTTF >= 0)
			{
				fontPath = FileUtils::sharedFileUtils()->fullPathForFilename(fontName.c_str());
				int nFindPos = fontName.rfind("/");
				fontName = &fontName[nFindPos + 1];

				nFindPos = fontName.rfind(".");
				fontName = fontName.substr(0, nFindPos); //纯ttf文件名
			}
			else
			{
				size_t nFindPos = fontName.rfind("/");
				if (nFindPos != fontName.npos)
				{
					if (fontName.length() == nFindPos + 1)
					{
						fontName = "";
					}
					else
					{
						fontName = &fontName[nFindPos + 1];
					}
				}
			}
			tNewFont.lfCharSet = DEFAULT_CHARSET;
			strcpy_s(tNewFont.lfFaceName, LF_FACESIZE, fontName.c_str());
		}

		if (nSize)
		{
			tNewFont.lfHeight = -nSize;
		}
		GetObjectA(m_hFont, sizeof(tOldFont), &tOldFont);

		if (tOldFont.lfHeight == tNewFont.lfHeight
			&& 0 == strcmp(tOldFont.lfFaceName, tNewFont.lfFaceName))
		{
			// already has the font 
			bRet = true;
			break;
		}

		// delete old font
		if (m_hFont != hDefFont)
		{
			DeleteObject(m_hFont);
			// release old font register
			if (m_curFontPath.size() > 0)
			{
				std::string fontPath = utf8Togbk(m_curFontPath.c_str());
				if (fontPath.length() > 0)
				{
					if (RemoveFontResource(fontPath.c_str()))
					{
						SendMessage(m_hWnd, WM_FONTCHANGE, 0, 0);
					}
				}
			}

			if (fontPath.size() > 0)
				m_curFontPath = fontPath;
			else
				m_curFontPath.clear();

			// register temp font
			if (m_curFontPath.size() > 0)
			{
				std::string fontPath = utf8Togbk(m_curFontPath.c_str());
				if (fontPath.length() > 0)
				{
					if (AddFontResource(fontPath.c_str()))
					{
						SendMessage(m_hWnd, WM_FONTCHANGE, 0, 0);
					}
				}
			}
		}
		m_hFont = NULL;

		// disable Cleartype
		tNewFont.lfQuality = ANTIALIASED_QUALITY;

		// create new font
		m_hFont = CreateFontIndirectA(&tNewFont);
		if (!m_hFont)
		{
			// create failed, use default font
			m_hFont = hDefFont;
			break;
		}

		bRet = true;
	} while (0);
	return bRet;
}


SIZE BitmapDC::sizeWithText(const char* pszText, int nLen, DWORD dwFmt, LONG nWidthLimit)
{
	SIZE tRet = { 0 };
	do
	{
		CC_BREAK_IF(!pszText || nLen <= 0);

		RECT rc = { 0, 0, 0, 0 };
		DWORD dwCalcFmt = DT_CALCRECT;

		if (nWidthLimit > 0)
		{
			rc.right = nWidthLimit;
			dwCalcFmt |= DT_WORDBREAK
				| (dwFmt & DT_CENTER)
				| (dwFmt & DT_RIGHT);
		}
		// use current font to measure text extent
		HGDIOBJ hOld = SelectObject(m_hDC, m_hFont);

		// measure text size
		DrawTextA(m_hDC, pszText, nLen, &rc, dwCalcFmt);
		SelectObject(m_hDC, hOld);

		tRet.cx = rc.right;
		tRet.cy = rc.bottom;
	} while (0);

	return tRet;
}


int BitmapDC::drawText(const char* pszText, SIZE& tSize, CCImage::ETextAlign eAlign)
{
	int nRet = 0;
	wchar_t* pwszBuffer = 0;
	do
	{
		CC_BREAK_IF(!pszText);

		DWORD dwFmt = DT_WORDBREAK;
		DWORD dwHoriFlag = eAlign & 0x0f;
		DWORD dwVertFlag = (eAlign & 0xf0) >> 4;

		switch (dwHoriFlag)
		{
		case 1: // left
			dwFmt |= DT_LEFT;
			break;
		case 2: // right
			dwFmt |= DT_RIGHT;
			break;
		case 3: // center
			dwFmt |= DT_CENTER;
			break;
		}

		std::string txtGbk = utf8Togbk(pszText);
		SIZE newSize = sizeWithText(txtGbk.c_str(), txtGbk.length(), dwFmt, tSize.cx);

		RECT rcText = { 0 };
		// if content width is 0, use text size as content size
		if (tSize.cx <= 0)
		{
			tSize = newSize;
			rcText.right = newSize.cx;
			rcText.bottom = newSize.cy;
		}
		else
		{
			LONG offsetX = 0;
			LONG offsetY = 0;
			rcText.right = newSize.cx; // store the text width to rectangle

			// calculate text horizontal offset
			if (1 != dwHoriFlag          // and text isn't align to left
				&& newSize.cx < tSize.cx)   // and text's width less then content width,
			{                               // then need adjust offset of X.
				offsetX = (2 == dwHoriFlag) ? tSize.cx - newSize.cx     // align to right
					: (tSize.cx - newSize.cx) / 2;                      // align to center
			}

			// if content height is 0, use text height as content height
			// else if content height less than text height, use content height to draw text
			if (tSize.cy <= 0)
			{
				tSize.cy = newSize.cy;
				dwFmt |= DT_NOCLIP;
				rcText.bottom = newSize.cy; // store the text height to rectangle
			}
			else if (tSize.cy < newSize.cy)
			{
				// content height larger than text height need, clip text to rect
				rcText.bottom = tSize.cy;
			}
			else
			{
				rcText.bottom = newSize.cy; // store the text height to rectangle

				// content larger than text, need adjust vertical position
				dwFmt |= DT_NOCLIP;

				// calculate text vertical offset
				offsetY = (2 == dwVertFlag) ? tSize.cy - newSize.cy     // align to bottom
					: (3 == dwVertFlag) ? (tSize.cy - newSize.cy) / 2   // align to middle
					: 0;                                                // align to top
			}

			if (offsetX || offsetY)
			{
				OffsetRect(&rcText, offsetX, offsetY);
			}
		}

		CC_BREAK_IF(!prepareBitmap(tSize.cx, tSize.cy));

		// draw text
		HGDIOBJ hOldFont = SelectObject(m_hDC, m_hFont);
		HGDIOBJ hOldBmp = SelectObject(m_hDC, m_hBmp);

		SetBkMode(m_hDC, TRANSPARENT);
		SetTextColor(m_hDC, RGB(255, 255, 255)); // white color

		// draw text
		nRet = DrawTextA(m_hDC, txtGbk.c_str(), txtGbk.length(), &rcText, dwFmt);
		//DrawTextA(m_hDC, pszText, nLen, &rcText, dwFmt);

		SelectObject(m_hDC, hOldBmp);
		SelectObject(m_hDC, hOldFont);
	} while (0);
	CC_SAFE_DELETE_ARRAY(pwszBuffer);
	return nRet;
}





//////////////////////////////////
// Image接口的平台实现
//
bool CCImage::initWithString(
	const char* pText,
	int             nWidth/* = 0*/,
	int             nHeight/* = 0*/,
	ETextAlign      eAlignMask/* = kAlignCenter*/,
	const char* pFontName/* = nil*/,
	int             nSize/* = 0*/)
{
	bool bRet = false;
	do
	{
		CC_BREAK_IF(!pText);

		BitmapDC& dc = sharedBitmapDC();

		if (!dc.setFont(pFontName, nSize))
		{
			CCLog("Can't found font(%s), use system default", pFontName);
		}

		// draw text
		SIZE size = { nWidth, nHeight };
		CC_BREAK_IF(!dc.drawText(pText, size, eAlignMask));

		m_pData = new unsigned char[size.cx * size.cy * 4];
		CC_BREAK_IF(!m_pData);

		struct
		{
			BITMAPINFOHEADER bmiHeader;
			int mask[4];
		} bi = { 0 };
		bi.bmiHeader.biSize = sizeof(bi.bmiHeader);
		CC_BREAK_IF(!GetDIBits(dc.getDC(), dc.getBitmap(), 0, 0,
			NULL, (LPBITMAPINFO)&bi, DIB_RGB_COLORS));

		m_nWidth = (short)size.cx;
		m_nHeight = (short)size.cy;
		m_bHasAlpha = true;
		m_bPreMulti = false;
		m_nBitsPerComponent = 8;
		// copy pixed data
		bi.bmiHeader.biHeight = (bi.bmiHeader.biHeight > 0)
			? -bi.bmiHeader.biHeight : bi.bmiHeader.biHeight;
		GetDIBits(dc.getDC(), dc.getBitmap(), 0, m_nHeight, m_pData,
			(LPBITMAPINFO)&bi, DIB_RGB_COLORS);

		// change pixel's alpha value to 255, when it's RGB != 0
		COLORREF* pPixel = NULL;
		for (int y = 0; y < m_nHeight; ++y)
		{
			pPixel = (COLORREF*)m_pData + y * m_nWidth;
			for (int x = 0; x < m_nWidth; ++x)
			{
				COLORREF& clr = *pPixel;

				clr |= (0xffffff | (GetRValue(clr) << 24));
				++pPixel;
			}
		}

		bRet = true;
	} while (0);

	return bRet;
}




NS_CC_END