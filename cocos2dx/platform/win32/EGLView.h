#pragma once

#include "EGLViewProtocol.h"


NS_CC_BEGIN



class CC_DLL EGLView : public EGLViewProtocol
{
public:
	EGLView();
	virtual ~EGLView();

	static EGLView* sharedOpenGLView();


public:
	virtual void    end();
	virtual bool    isOpenGLReady();
	virtual void    swapBuffers();


public:
	virtual void	setViewPortInPoints(float x, float y, float w, float h);
	virtual void	setScissorInPoints(float x, float y, float w, float h);

	virtual void	setFrameSize(float width, float height);

	virtual void	resize(int width, int height);
	virtual void	centerWindow();

	virtual LRESULT windowProc(UINT message, WPARAM wParam, LPARAM lParam);

	// 加速器的键盘钩子
	typedef void (*LPFN_ACCELEROMETER_KEYHOOK)(UINT message, WPARAM wParam, LPARAM lParam);
	void setAccelerometerKeyHook(LPFN_ACCELEROMETER_KEYHOOK lpfnAccelerometerKeyHook);

public:
	HWND			getHWnd();

	void			setFrameZoomFactor(float fZoomFactor);
	float			getFrameZoomFactor();

protected:
	bool			initGL();
	void			destroyGL();

protected:
	virtual bool	create();


protected:
	static EGLView*	s_pEglView;
	LPCSTR			_menu;

	HWND			_hWnd;
	HDC				_hDC;
	HGLRC			_hRC;

	float			_fFrameZoomFactor;

	bool			_bSupportTouch;
	bool			_bCaptured;

	LPFN_ACCELEROMETER_KEYHOOK m_lpfnAccelerometerKeyHook;
};



NS_CC_END

