#pragma once
#include "ccMacros.h"
#include "cocoa/Set.h"



NS_CC_BEGIN

enum {
	CCTOUCHBEGAN,
	CCTOUCHMOVED,
	CCTOUCHENDED,
	CCTOUCHCANCELLED,

	ccTouchMax,
};


class TouchEvent;
class TouchDelegate;
class TouchHandler;
class CC_DLL EGLTouchDelegate
{
public:
    virtual void touchesBegan(Set* touches, TouchEvent* pEvent) = 0;
    virtual void touchesMoved(Set* touches, TouchEvent* pEvent) = 0;
    virtual void touchesEnded(Set* touches, TouchEvent* pEvent) = 0;
    virtual void touchesCancelled(Set* touches, TouchEvent* pEvent) = 0;
    virtual ~EGLTouchDelegate() {}
};


class CC_DLL TouchDispatcher : public Object, public EGLTouchDelegate
{
public:
    TouchDispatcher();
    ~TouchDispatcher();

	bool		isDispatchEvents(void);
	void		setDispatchEvents(bool bDispatchEvents);

	void		addTargetedDelegate(TouchDelegate* pDelegate, int nPriority, bool bSwallowsTouches);
	void		addStandardDelegate(TouchDelegate* pDelegate, int nPriority);

	void		removeDelegate(TouchDelegate* pDelegate);
	void		removeAllDelegates();

protected:
	void		forceAddHandler(TouchHandler* pHandler, Array* pArray);
	void		forceRemoveDelegate(TouchDelegate* pDelegate);
	void		forceRemoveAllDelegates(void);

	void		rearrangeHandlers(Array* pArray);

	TouchHandler* findHandler(TouchDelegate* pDelegate);
	TouchHandler* findHandler(Array* pArray, TouchDelegate* pDelegate);

public:
	void		setPriority(int nPriority, TouchDelegate* pDelegate);
	

public:
    virtual void touchesBegan(Set* touches, TouchEvent* pEvent);
    virtual void touchesMoved(Set* touches, TouchEvent* pEvent);
    virtual void touchesEnded(Set* touches, TouchEvent* pEvent);
    virtual void touchesCancelled(Set* touches, TouchEvent* pEvent);

private:
	void		 touches(Set* pTouches, TouchEvent* pEvent, unsigned int uIndex);


protected:
	bool		_bDispatchEvents;

	Array*		_pOneHandlers;
	Array*		_pMultiHandlers;

	Array*		_pHandlersToAdd;			//新增时 (delegate + priority) 所以封装成handler来保存
	struct _ccCArray* _pDelegateToRemove;  //删除时 delegate

	bool		_bLocked;
	bool		_bToAdd;
	bool		_bToRemove;

	bool		_bToQuit;

	int			_arrHandlerHelperData[ccTouchMax];
};


NS_CC_END
