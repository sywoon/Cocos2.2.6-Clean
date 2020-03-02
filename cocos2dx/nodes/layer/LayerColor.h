#pragma once
#include "ccMacros.h"
#include "ccProtocol.h"
#include "ccClassMacros.h"
#include "LayerRBGA.h"


NS_CC_BEGIN


class CC_DLL LayerColor : public LayerRGBA, public BlendProtocol
{
public:
    LayerColor();
    virtual ~LayerColor();

    static LayerColor*  create();
    static LayerColor*  create(const ccColor4B& color, GLfloat width, GLfloat height);
    static LayerColor*  create(const ccColor4B& color);

protected:
    virtual bool        init();
    virtual bool        initWithColor(const ccColor4B& color, GLfloat width, GLfloat height);
    virtual bool        initWithColor(const ccColor4B& color);

protected:
	virtual void        updateColor();

public:
    void                changeWidth(GLfloat w);
    void                changeHeight(GLfloat h);
    void                changeWidthAndHeight(GLfloat w, GLfloat h);

	virtual void        setColor(const ccColor3B& color);
	virtual void        setOpacity(GLubyte opacity);

public:
    virtual void        setBlendFunc(ccBlendFunc blendFunc);
    virtual ccBlendFunc getBlendFunc(void);

public:
	virtual void        draw();
	virtual void        setContentSize(const Size& var);


protected:
    ccBlendFunc     _tBlendFunc;

	ccVertex2F      _pSquareVertices[4];
	ccColor4F       _pSquareColors[4];
};



NS_CC_END