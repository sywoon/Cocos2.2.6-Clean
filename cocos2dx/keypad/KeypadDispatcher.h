#pragma once

#include "ccMacros.h"
#include "cocoa/Object.h"


NS_CC_BEGIN


typedef enum {
	kTypeBackClicked = 1,
	kTypeMenuClicked,
} ccKeypadMSGType;


class Array;
class KeypadDelegate;
class CC_DLL KeypadDispatcher : public Object
{
public:
	KeypadDispatcher();
	~KeypadDispatcher();


    void addDelegate(KeypadDelegate* pDelegate);
    void removeDelegate(KeypadDelegate* pDelegate);

    void forceAddDelegate(KeypadDelegate* pDelegate);
    void forceRemoveDelegate(KeypadDelegate* pDelegate);

    bool dispatchKeypadMSG(ccKeypadMSGType nMsgType);


protected:
	Array*	_pDelegates;

	bool	_bLocked;
	bool	_bToAdd;
	bool	_bToRemove;

	struct _ccCArray* _pHandlersToAdd;
	struct _ccCArray* _pHandlersToRemove;
};


NS_CC_END