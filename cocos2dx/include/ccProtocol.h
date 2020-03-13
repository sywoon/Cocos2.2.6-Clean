#pragma once

#include "ccMacros.h"
#include "ccType.h"


NS_CC_BEGIN


class CC_DLL RGBAProtocol
{
public:
    virtual void             setColor(const ccColor3B& color) = 0;
    virtual const ccColor3B& getColor(void) = 0;
    
    // from 0 to 255, where 255 means fully opaque and 0 means fully transparent.
    virtual void    setOpacity(GLubyte opacity) = 0;
    virtual GLubyte getOpacity(void) = 0;

    // If thie property is set to true, then the rendered color will be affected by opacity.
    // Normally, r = r * opacity/255, g = g * opacity/255, b = b * opacity/255.
    // bValue  true then the opacity will be applied as: glColor(R,G,B,opacity);
    //         false then the opacity will be applied as: glColor(opacity, opacity, opacity, opacity);
    virtual void    setOpacityModifyRGB(bool bValue) = 0;

    // Returns whether or not the opacity will be applied using glColor(R,G,B,opacity)
    // or glColor(opacity, opacity, opacity, opacity)
    virtual bool    isOpacityModifyRGB(void) = 0;

    // whether or not color should be propagated to its children.
    virtual bool    isCascadeColorEnabled(void) = 0;
    virtual void    setCascadeColorEnabled(bool cascadeColorEnabled) = 0;
    virtual const ccColor3B& getDisplayedColor(void) = 0;
    virtual void    updateDisplayedColor(const ccColor3B& color) = 0;
    

    virtual bool    isCascadeOpacityEnabled(void) = 0;
    virtual void    setCascadeOpacityEnabled(bool cascadeOpacityEnabled) = 0;
    virtual GLubyte getDisplayedOpacity(void) = 0;
    virtual void    updateDisplayedOpacity(GLubyte opacity) = 0;
};


class CC_DLL BlendProtocol
{
public:
    // blendFunc A structure with source and destination factor to specify pixel arithmetic,
    //               e.g. {GL_ONE, GL_ONE}, {GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA}.
    virtual void        setBlendFunc(ccBlendFunc blendFunc) = 0;

    virtual ccBlendFunc getBlendFunc(void) = 0;
};


class Texture2D;
class CC_DLL TextureProtocol : public BlendProtocol
{
public:
    virtual Texture2D* getTexture(void) = 0;

    virtual void setTexture(Texture2D* texture) = 0;
};





NS_CC_END