#pragma once
#include "ccMacros.h"
#include "cocoa/Set.h"



NS_CC_BEGIN


class TouchEvent;
class CC_DLL EGLTouchDelegate
{
public:
    virtual void touchesBegan(Set* touches, TouchEvent* pEvent) = 0;
    virtual void touchesMoved(Set* touches, TouchEvent* pEvent) = 0;
    virtual void touchesEnded(Set* touches, TouchEvent* pEvent) = 0;
    virtual void touchesCancelled(Set* touches, TouchEvent* pEvent) = 0;
    virtual ~EGLTouchDelegate() {}
};




NS_CC_END
