#pragma once
#include "ccMacros.h"
#include "ccClassMacros.h"
#include "Layer.h"
#include "ccProtocol.h"
#include "ccType.h"


NS_CC_BEGIN

class CC_DLL LayerRGBA : public Layer, public RGBAProtocol
{
public:
    CREATE_FUNC(LayerRGBA);
    LayerRGBA();
	virtual ~LayerRGBA();

protected:
	virtual bool		init();

public:
	virtual void		setColor(const ccColor3B& color);
	virtual const ccColor3B& getColor(void);

	virtual void		setOpacity(GLubyte opacity);
	virtual GLubyte		getOpacity(void);

	virtual void		setOpacityModifyRGB(bool bValue);
	virtual bool		isOpacityModifyRGB(void);

	virtual bool		isCascadeColorEnabled(void);
	virtual void		setCascadeColorEnabled(bool cascadeColorEnabled);

	virtual void		updateDisplayedColor(const ccColor3B& color);
	virtual const ccColor3B& getDisplayedColor(void);

	virtual bool		isCascadeOpacityEnabled(void);
	virtual void		setCascadeOpacityEnabled(bool cascadeOpacityEnabled);

	virtual void		updateDisplayedOpacity(GLubyte opacity);
	virtual GLubyte		getDisplayedOpacity(void);


protected:
	GLubyte		_displayedOpacity;
	GLubyte		_realOpacity;

	ccColor3B	_displayedColor;
	ccColor3B	_realColor;

	bool		_cascadeOpacityEnabled;
	bool		_cascadeColorEnabled;
};



NS_CC_END