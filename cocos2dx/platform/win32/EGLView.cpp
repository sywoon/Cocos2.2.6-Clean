#include <stdio.h>
#include "EGLView.h"
#include "GL/glew.h"
#include "platform/PlatformBase.h"
#include "Director.h"



NS_CC_BEGIN

static EGLView* s_pMainWindow = NULL;
static const char* kWindowClassName = "Cocos2dxWin32";
EGLView* EGLView::s_pEglView = NULL;

static void SetupPixelFormat(HDC hDC)
{
	int pixelFormat;

	PIXELFORMATDESCRIPTOR pfd =
	{
		sizeof(PIXELFORMATDESCRIPTOR),  // size
		1,                          // version
		PFD_SUPPORT_OPENGL |        // OpenGL window
		PFD_DRAW_TO_WINDOW |        // render to window
		PFD_DOUBLEBUFFER,           // support double-buffering
		PFD_TYPE_RGBA,              // color type
		32,                         // preferred color depth
		0, 0, 0, 0, 0, 0,           // color bits (ignored)
		0,                          // no alpha buffer
		0,                          // alpha bits (ignored)
		0,                          // no accumulation buffer
		0, 0, 0, 0,                 // accum bits (ignored)
		24,                         // depth buffer
		8,                          // no stencil buffer
		0,                          // no auxiliary buffers
		PFD_MAIN_PLANE,             // main layer
		0,                          // reserved
		0, 0, 0,                    // no layer, visible, damage masks
	};

	pixelFormat = ChoosePixelFormat(hDC, &pfd);
	SetPixelFormat(hDC, pixelFormat, &pfd);
}

// framebuffer的支持 可以从扩展中获取接口
static bool glew_dynamic_binding()
{
	const char* gl_extensions = (const char*)glGetString(GL_EXTENSIONS);

	// If the current opengl driver doesn't have framebuffers methods, check if an extension exists
	if (glGenFramebuffers == NULL)
	{
		CCLog("OpenGL: glGenFramebuffers is NULL, try to detect an extension");
		if (strstr(gl_extensions, "ARB_framebuffer_object"))
		{
			CCLog("OpenGL: ARB_framebuffer_object is supported");

			glIsRenderbuffer = (PFNGLISRENDERBUFFERPROC)wglGetProcAddress("glIsRenderbuffer");
			glBindRenderbuffer = (PFNGLBINDRENDERBUFFERPROC)wglGetProcAddress("glBindRenderbuffer");
			glDeleteRenderbuffers = (PFNGLDELETERENDERBUFFERSPROC)wglGetProcAddress("glDeleteRenderbuffers");
			glGenRenderbuffers = (PFNGLGENRENDERBUFFERSPROC)wglGetProcAddress("glGenRenderbuffers");
			glRenderbufferStorage = (PFNGLRENDERBUFFERSTORAGEPROC)wglGetProcAddress("glRenderbufferStorage");
			glGetRenderbufferParameteriv = (PFNGLGETRENDERBUFFERPARAMETERIVPROC)wglGetProcAddress("glGetRenderbufferParameteriv");
			glIsFramebuffer = (PFNGLISFRAMEBUFFERPROC)wglGetProcAddress("glIsFramebuffer");
			glBindFramebuffer = (PFNGLBINDFRAMEBUFFERPROC)wglGetProcAddress("glBindFramebuffer");
			glDeleteFramebuffers = (PFNGLDELETEFRAMEBUFFERSPROC)wglGetProcAddress("glDeleteFramebuffers");
			glGenFramebuffers = (PFNGLGENFRAMEBUFFERSPROC)wglGetProcAddress("glGenFramebuffers");
			glCheckFramebufferStatus = (PFNGLCHECKFRAMEBUFFERSTATUSPROC)wglGetProcAddress("glCheckFramebufferStatus");
			glFramebufferTexture1D = (PFNGLFRAMEBUFFERTEXTURE1DPROC)wglGetProcAddress("glFramebufferTexture1D");
			glFramebufferTexture2D = (PFNGLFRAMEBUFFERTEXTURE2DPROC)wglGetProcAddress("glFramebufferTexture2D");
			glFramebufferTexture3D = (PFNGLFRAMEBUFFERTEXTURE3DPROC)wglGetProcAddress("glFramebufferTexture3D");
			glFramebufferRenderbuffer = (PFNGLFRAMEBUFFERRENDERBUFFERPROC)wglGetProcAddress("glFramebufferRenderbuffer");
			glGetFramebufferAttachmentParameteriv = (PFNGLGETFRAMEBUFFERATTACHMENTPARAMETERIVPROC)wglGetProcAddress("glGetFramebufferAttachmentParameteriv");
			glGenerateMipmap = (PFNGLGENERATEMIPMAPPROC)wglGetProcAddress("glGenerateMipmap");
		}
		else if (strstr(gl_extensions, "EXT_framebuffer_object"))
		{
			CCLog("OpenGL: EXT_framebuffer_object is supported");
			glIsRenderbuffer = (PFNGLISRENDERBUFFERPROC)wglGetProcAddress("glIsRenderbufferEXT");
			glBindRenderbuffer = (PFNGLBINDRENDERBUFFERPROC)wglGetProcAddress("glBindRenderbufferEXT");
			glDeleteRenderbuffers = (PFNGLDELETERENDERBUFFERSPROC)wglGetProcAddress("glDeleteRenderbuffersEXT");
			glGenRenderbuffers = (PFNGLGENRENDERBUFFERSPROC)wglGetProcAddress("glGenRenderbuffersEXT");
			glRenderbufferStorage = (PFNGLRENDERBUFFERSTORAGEPROC)wglGetProcAddress("glRenderbufferStorageEXT");
			glGetRenderbufferParameteriv = (PFNGLGETRENDERBUFFERPARAMETERIVPROC)wglGetProcAddress("glGetRenderbufferParameterivEXT");
			glIsFramebuffer = (PFNGLISFRAMEBUFFERPROC)wglGetProcAddress("glIsFramebufferEXT");
			glBindFramebuffer = (PFNGLBINDFRAMEBUFFERPROC)wglGetProcAddress("glBindFramebufferEXT");
			glDeleteFramebuffers = (PFNGLDELETEFRAMEBUFFERSPROC)wglGetProcAddress("glDeleteFramebuffersEXT");
			glGenFramebuffers = (PFNGLGENFRAMEBUFFERSPROC)wglGetProcAddress("glGenFramebuffersEXT");
			glCheckFramebufferStatus = (PFNGLCHECKFRAMEBUFFERSTATUSPROC)wglGetProcAddress("glCheckFramebufferStatusEXT");
			glFramebufferTexture1D = (PFNGLFRAMEBUFFERTEXTURE1DPROC)wglGetProcAddress("glFramebufferTexture1DEXT");
			glFramebufferTexture2D = (PFNGLFRAMEBUFFERTEXTURE2DPROC)wglGetProcAddress("glFramebufferTexture2DEXT");
			glFramebufferTexture3D = (PFNGLFRAMEBUFFERTEXTURE3DPROC)wglGetProcAddress("glFramebufferTexture3DEXT");
			glFramebufferRenderbuffer = (PFNGLFRAMEBUFFERRENDERBUFFERPROC)wglGetProcAddress("glFramebufferRenderbufferEXT");
			glGetFramebufferAttachmentParameteriv = (PFNGLGETFRAMEBUFFERATTACHMENTPARAMETERIVPROC)wglGetProcAddress("glGetFramebufferAttachmentParameterivEXT");
			glGenerateMipmap = (PFNGLGENERATEMIPMAPPROC)wglGetProcAddress("glGenerateMipmapEXT");
		}
		else
		{
			CCLog("OpenGL: No framebuffers extension is supported");
			CCLog("OpenGL: Any call to Fbo will crash!");
			return false;
		}
	}
	return true;
}


static LRESULT CALLBACK _WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (s_pMainWindow && s_pMainWindow->getHWnd() == hWnd)
	{
		return s_pMainWindow->windowProc(uMsg, wParam, lParam);
	}
	else
	{
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}
}

EGLView* EGLView::sharedOpenGLView()
{

	if (s_pEglView == NULL)
	{
		s_pEglView = new EGLView();
		if (!s_pEglView->create())
		{
			delete s_pEglView;
			s_pEglView = NULL;
		}
	}

	return s_pEglView;
}

EGLView::EGLView()
	: _bCaptured(false)
	, _hWnd(NULL)
	, _hDC(NULL)
	, _hRC(NULL)
	, _menu(NULL)
	, _fFrameZoomFactor(1.0f)
{
	strcpy(_szViewName, "Cocos2dxWin32");
}

EGLView::~EGLView()
{
}


bool EGLView::initGL()
{
	_hDC = GetDC(_hWnd);
	SetupPixelFormat(_hDC);
	
	_hRC = wglCreateContext(_hDC);
	wglMakeCurrent(_hDC, _hRC);

	// check OpenGL version at first
	const GLubyte* glVersion = glGetString(GL_VERSION);
	CCLOG("OpenGL version = %s", glVersion);

	if (atof((const char*)glVersion) < 1.5)
	{
		char strComplain[256] = { 0 };
		sprintf(strComplain,
			"OpenGL 1.5 or higher is required (your version is %s). Please upgrade the driver of your video card.",
			glVersion);
		CCMessageBox(strComplain, "OpenGL version too old");
		return false;
	}

	GLenum GlewInitResult = glewInit();
	if (GLEW_OK != GlewInitResult)
	{
		CCMessageBox((char*)glewGetErrorString(GlewInitResult), "OpenGL error");
		return false;
	}

	if (GLEW_ARB_vertex_shader && GLEW_ARB_fragment_shader)
	{
		CCLog("Ready for GLSL");
	}
	else
	{
		CCLog("Not totally ready :(");
	}

	if (glewIsSupported("GL_VERSION_2_0"))
	{
		CCLog("Ready for OpenGL 2.0");
	}
	else
	{
		CCLog("OpenGL 2.0 not supported");
	}

	if (glew_dynamic_binding() == false)
	{
		CCMessageBox("No OpenGL framebuffer support. Please upgrade the driver of your video card.", "OpenGL error");
		return false;
	}

	// Enable point size by default on windows.
	glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);

	return true;
}

void EGLView::destroyGL()
{
	if (_hDC != NULL && _hRC != NULL)
	{
		// deselect rendering context and delete it
		wglMakeCurrent(_hDC, NULL);
		wglDeleteContext(_hRC);
	}
}


bool EGLView::create()
{
	bool bRet = false;
	do
	{
		CC_BREAK_IF(_hWnd);

		HINSTANCE hInstance = GetModuleHandle(NULL);
		WNDCLASS  wc; 

		// Redraw On Size, And Own DC For Window.
		wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
		wc.lpfnWndProc = _WindowProc;
		wc.cbClsExtra = 0;
		wc.cbWndExtra = 0;
		wc.hInstance = hInstance; 
		wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);
		wc.hCursor = LoadCursor(NULL, IDC_ARROW);
		wc.hbrBackground = NULL;                           // No Background Required For GL
		wc.lpszMenuName = _menu; 
		wc.lpszClassName = kWindowClassName;

		CC_BREAK_IF(!RegisterClass(&wc) && 1410 != GetLastError());

		RECT rcDesktop;
		GetWindowRect(GetDesktopWindow(), &rcDesktop);

		// create window
		_hWnd = CreateWindowEx(
			WS_EX_APPWINDOW | WS_EX_WINDOWEDGE,    // Extended Style For The Window
			kWindowClassName,                                    // Class Name
			_szViewName,                                         // Window Title
			WS_CAPTION | WS_POPUPWINDOW | WS_MINIMIZEBOX,        // Defined Window Style
			0, 0,                                                // Window Position
			//TODO: Initializing width with a large value to avoid getting a wrong client area by 'GetClientRect' function.
			1000,                                               // Window Width
			1000,                                               // Window Height
			NULL,                                                // No Parent Window
			NULL,                                                // No Menu
			hInstance,                                            // Instance
			NULL);

		CC_BREAK_IF(!_hWnd);

		bRet = initGL();
		if (!bRet) destroyGL();
		CC_BREAK_IF(!bRet);

		s_pMainWindow = this;
		bRet = true;
	} while (0);

	return bRet;
}

HWND EGLView::getHWnd()
{
	return _hWnd;
}

bool EGLView::isOpenGLReady()
{
	return (_hDC != NULL && _hRC != NULL);
}

void EGLView::end()
{
	if (_hWnd)
	{
		DestroyWindow(_hWnd);
		_hWnd = NULL;
	}
	s_pMainWindow = NULL;
	UnregisterClass(kWindowClassName, GetModuleHandle(NULL));
	delete this;
}

void EGLView::swapBuffers()
{
	if (_hDC != NULL)
	{
		::SwapBuffers(_hDC);
	}
}


void EGLView::setViewPortInPoints(float x, float y, float w, float h)
{
	glViewport((GLint)(x * _fScaleX * _fFrameZoomFactor + _rectViewPort.origin.x * _fFrameZoomFactor),
		(GLint)(y * _fScaleY * _fFrameZoomFactor + _rectViewPort.origin.y * _fFrameZoomFactor),
		(GLsizei)(w * _fScaleX * _fFrameZoomFactor),
		(GLsizei)(h * _fScaleY * _fFrameZoomFactor));
}

void EGLView::setScissorInPoints(float x, float y, float w, float h)
{
	glScissor((GLint)(x * _fScaleX * _fFrameZoomFactor + _rectViewPort.origin.x * _fFrameZoomFactor),
		(GLint)(y * _fScaleY * _fFrameZoomFactor + _rectViewPort.origin.y * _fFrameZoomFactor),
		(GLsizei)(w * _fScaleX * _fFrameZoomFactor),
		(GLsizei)(h * _fScaleY * _fFrameZoomFactor));
}


void EGLView::setFrameZoomFactor(float fZoomFactor)
{
	_fFrameZoomFactor = fZoomFactor;
	resize((int)(_sizeFrame.width * fZoomFactor), (int)(_sizeFrame.height * fZoomFactor));
	centerWindow();
}

float EGLView::getFrameZoomFactor()
{
	return _fFrameZoomFactor;
}

void EGLView::setFrameSize(float width, float height)
{
	EGLViewProtocol::setFrameSize(width, height);

	resize((int)width, (int)height);	// adjust window size for menubar
	centerWindow();
}

void EGLView::resize(int width, int height)
{
	if (!_hWnd)
		return;

	RECT rcWindow;
	GetWindowRect(_hWnd, &rcWindow);

	RECT rcClient;
	GetClientRect(_hWnd, &rcClient);

	// calculate new window width and height
	POINT ptDiff;
	ptDiff.x = (rcWindow.right - rcWindow.left) - rcClient.right;
	ptDiff.y = (rcWindow.bottom - rcWindow.top) - rcClient.bottom;
	rcClient.right = rcClient.left + width;
	rcClient.bottom = rcClient.top + height;

	const Size & frameSize = getFrameSize();
	if (frameSize.width > 0)
	{
		SetWindowText(_hWnd, _szViewName);
	}

	AdjustWindowRectEx(&rcClient, GetWindowLong(_hWnd, GWL_STYLE), FALSE, GetWindowLong(_hWnd, GWL_EXSTYLE));

	// change width and height
	SetWindowPos(_hWnd, 0, 0, 0, width + ptDiff.x, height + ptDiff.y,
			SWP_NOCOPYBITS | SWP_NOMOVE | SWP_NOOWNERZORDER | SWP_NOZORDER);
}

void EGLView::centerWindow()
{
	if (!_hWnd)
		return;

	RECT rcDesktop, rcWindow;
	GetWindowRect(GetDesktopWindow(), &rcDesktop);

	// substract the task bar
	HWND hTaskBar = FindWindow(TEXT("Shell_TrayWnd"), NULL);
	if (hTaskBar != NULL)
	{
		APPBARDATA abd;

		abd.cbSize = sizeof(APPBARDATA);
		abd.hWnd = hTaskBar;

		SHAppBarMessage(ABM_GETTASKBARPOS, &abd);
		SubtractRect(&rcDesktop, &rcDesktop, &abd.rc);
	}
	GetWindowRect(_hWnd, &rcWindow);

	int offsetX = rcDesktop.left + (rcDesktop.right - rcDesktop.left - (rcWindow.right - rcWindow.left)) / 2;
	offsetX = (offsetX > 0) ? offsetX : rcDesktop.left;

	int offsetY = rcDesktop.top + (rcDesktop.bottom - rcDesktop.top - (rcWindow.bottom - rcWindow.top)) / 2;
	offsetY = (offsetY > 0) ? offsetY : rcDesktop.top;

	SetWindowPos(_hWnd, 0, offsetX, offsetY, 0, 0, SWP_NOCOPYBITS | SWP_NOSIZE | SWP_NOOWNERZORDER | SWP_NOZORDER);
}


LRESULT EGLView::windowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	BOOL bProcessed = FALSE;

	switch (message)
	{
	case WM_LBUTTONDOWN:
		break;

	case WM_MOUSEMOVE:
		break;

	case WM_LBUTTONUP:
		break;

#if(_MSC_VER >= 1600)
	case WM_TOUCH:
	{
		BOOL bHandled = FALSE;
		UINT cInputs = LOWORD(wParam);
		PTOUCHINPUT pInputs = new TOUCHINPUT[cInputs];
		if (pInputs)
		{
			delete[] pInputs;
		}
	}
	break;
#endif /* #if(_MSC_VER >= 1600) */

	case WM_SIZE:
		switch (wParam)
		{
		case SIZE_RESTORED:
			Application::sharedApplication()->applicationWillEnterForeground();
			break;
		case SIZE_MINIMIZED:
			Application::sharedApplication()->applicationDidEnterBackground();
			break;
		}
		break;
	case WM_KEYDOWN:
		break;
	case WM_KEYUP:
		break;
	case WM_CHAR:
		break;
	case WM_PAINT:
		PAINTSTRUCT ps;
		BeginPaint(_hWnd, &ps);
		EndPaint(_hWnd, &ps);
		break;

	case WM_CLOSE:
		Director::sharedDirector()->end();
		break;

	case WM_DESTROY:
		destroyGL();
		PostQuitMessage(0);
		break;

	default:
		return DefWindowProc(_hWnd, message, wParam, lParam);
	}

	return 0;
}


NS_CC_END