#pragma once


#include "ccMacros.h"
#include "Node.h"


NS_CC_BEGIN


class CC_DLL NodeRGBA : public Node, public RGBAProtocol
{
public:
    NodeRGBA();
    virtual ~NodeRGBA();

    static NodeRGBA* create(void);

public:
	virtual bool	init();

	virtual void	setColor(const ccColor3B& color);
	virtual const ccColor3B& getColor(void);

	virtual void	setOpacity(GLubyte opacity);
	virtual GLubyte getOpacity();

	virtual void	setOpacityModifyRGB(bool bValue) { CC_UNUSED_PARAM(bValue); };
	virtual bool	isOpacityModifyRGB() { return false; };

	virtual bool	isCascadeColorEnabled();
	virtual void	setCascadeColorEnabled(bool cascadeColorEnabled);
	virtual const ccColor3B& getDisplayedColor();
	virtual void	updateDisplayedColor(const ccColor3B& parentColor);

	virtual bool	isCascadeOpacityEnabled();
	virtual void	setCascadeOpacityEnabled(bool cascadeOpacityEnabled);
	virtual GLubyte getDisplayedOpacity();
	virtual void	updateDisplayedOpacity(GLubyte parentOpacity);
	


protected:
	GLubyte		_displayedOpacity;
	GLubyte     _realOpacity;

	ccColor3B	_displayedColor;
	ccColor3B   _realColor;

	bool		_cascadeColorEnabled;
	bool        _cascadeOpacityEnabled;
};




NS_CC_END