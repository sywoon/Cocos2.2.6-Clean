#include "LayerRBGA.h"
#include "cocoa/Array.h"


NS_CC_BEGIN


LayerRGBA::LayerRGBA()
	: _displayedOpacity(255)
	, _realOpacity(255)

	, _displayedColor(ccWHITE)
	, _realColor(ccWHITE)

	, _cascadeOpacityEnabled(false)
	, _cascadeColorEnabled(false)
{}

LayerRGBA::~LayerRGBA() 
{}

bool LayerRGBA::init()
{
	if (!Layer::init())
		return false;

	setCascadeOpacityEnabled(false);
	setCascadeColorEnabled(false);

	return true;
}


void LayerRGBA::setColor(const ccColor3B& color)
{
	_displayedColor = _realColor = color;

	if (_cascadeColorEnabled)
	{
		ccColor3B parentColor = ccWHITE;
		RGBAProtocol* parent = dynamic_cast<RGBAProtocol*>(_pParent);
		if (parent && parent->isCascadeColorEnabled())
		{
			parentColor = parent->getDisplayedColor();
		}

		updateDisplayedColor(parentColor);
	}
}

const ccColor3B& LayerRGBA::getColor()
{
	return _realColor;
}

const ccColor3B& LayerRGBA::getDisplayedColor()
{
	return _displayedColor;
}

void LayerRGBA::setOpacity(GLubyte opacity)
{
	_displayedOpacity = _realOpacity = opacity;

	if (_cascadeOpacityEnabled)
	{
		GLubyte parentOpacity = 255;
		RGBAProtocol* parent = dynamic_cast<RGBAProtocol*>(_pParent);
		if (parent && parent->isCascadeOpacityEnabled())
		{
			parentOpacity = parent->getDisplayedOpacity();
		}
		updateDisplayedOpacity(parentOpacity);
	}
}

GLubyte LayerRGBA::getOpacity()
{
	return _realOpacity;
}

void LayerRGBA::setOpacityModifyRGB(bool bValue)
{
	CC_UNUSED_PARAM(bValue);
}

bool LayerRGBA::isOpacityModifyRGB(void)
{
	return false;
}


bool LayerRGBA::isCascadeColorEnabled()
{
	return _cascadeColorEnabled;
}

void LayerRGBA::setCascadeColorEnabled(bool cascadeColorEnabled)
{
	_cascadeColorEnabled = cascadeColorEnabled;
}

void LayerRGBA::updateDisplayedColor(const ccColor3B& parentColor)
{
	_displayedColor.r = _realColor.r * parentColor.r / 255.0;
	_displayedColor.g = _realColor.g * parentColor.g / 255.0;
	_displayedColor.b = _realColor.b * parentColor.b / 255.0;

	if (_cascadeColorEnabled)
	{
		Object* obj = NULL;
		ARRAY_FOREACH(_pChildren, obj)
		{
			RGBAProtocol* item = dynamic_cast<RGBAProtocol*>(obj);
			if (item)
			{
				item->updateDisplayedColor(_displayedColor);
			}
		}
	}
}


bool LayerRGBA::isCascadeOpacityEnabled()
{
	return _cascadeOpacityEnabled;
}

void LayerRGBA::setCascadeOpacityEnabled(bool cascadeOpacityEnabled)
{
	_cascadeOpacityEnabled = cascadeOpacityEnabled;
}


void LayerRGBA::updateDisplayedOpacity(GLubyte parentOpacity)
{
	_displayedOpacity = _realOpacity * parentOpacity / 255.0;

	if (_cascadeOpacityEnabled)
	{
		Object* obj = NULL;
		ARRAY_FOREACH(_pChildren, obj)
		{
			RGBAProtocol* item = dynamic_cast<RGBAProtocol*>(obj);
			if (item)
			{
				item->updateDisplayedOpacity(_displayedOpacity);
			}
		}
	}
}

GLubyte LayerRGBA::getDisplayedOpacity()
{
	return _displayedOpacity;
}



NS_CC_END