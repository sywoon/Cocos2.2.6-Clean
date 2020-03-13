#include "NodeRGBA.h"




NS_CC_BEGIN

NodeRGBA* NodeRGBA::create(void)
{
	NodeRGBA* pRet = new NodeRGBA();
	if (pRet && pRet->init())
	{
		pRet->autorelease();
	}
	else
	{
		CC_SAFE_DELETE(pRet);
	}
	return pRet;
}

NodeRGBA::NodeRGBA()
	: _displayedOpacity(255)
	, _realOpacity(255)

	, _displayedColor(ccWHITE)
	, _realColor(ccWHITE)

	, _cascadeColorEnabled(false)
	, _cascadeOpacityEnabled(false)
{}

NodeRGBA::~NodeRGBA() 
{}

bool NodeRGBA::init()
{
	if (Node::init())
	{
		_displayedOpacity = _realOpacity = 255;
		_displayedColor = _realColor = ccWHITE;
		_cascadeOpacityEnabled = _cascadeColorEnabled = false;
		return true;
	}
	return false;
}

void NodeRGBA::setColor(const ccColor3B& color)
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

const ccColor3B& NodeRGBA::getColor(void)
{
	return _realColor;
}

void NodeRGBA::setOpacity(GLubyte opacity)
{
	_displayedOpacity = _realOpacity = opacity;

	if (_cascadeOpacityEnabled)
	{
		GLubyte parentOpacity = 255;
		RGBAProtocol* pParent = dynamic_cast<RGBAProtocol*>(_pParent);
		if (pParent && pParent->isCascadeOpacityEnabled())
		{
			parentOpacity = pParent->getDisplayedOpacity();
		}
		this->updateDisplayedOpacity(parentOpacity);
	}
}

GLubyte NodeRGBA::getOpacity(void)
{
	return _realOpacity;
}


bool NodeRGBA::isCascadeColorEnabled(void)
{
	return _cascadeColorEnabled;
}

void NodeRGBA::setCascadeColorEnabled(bool cascadeColorEnabled)
{
	_cascadeColorEnabled = cascadeColorEnabled;
}

const ccColor3B& NodeRGBA::getDisplayedColor()
{
	return _displayedColor;
}

void NodeRGBA::updateDisplayedColor(const ccColor3B& parentColor)
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


bool NodeRGBA::isCascadeOpacityEnabled(void)
{
	return _cascadeOpacityEnabled;
}

void NodeRGBA::setCascadeOpacityEnabled(bool cascadeOpacityEnabled)
{
	_cascadeOpacityEnabled = cascadeOpacityEnabled;
}

GLubyte NodeRGBA::getDisplayedOpacity(void)
{
	return _displayedOpacity;
}

void NodeRGBA::updateDisplayedOpacity(GLubyte parentOpacity)
{
	_displayedOpacity = _realOpacity * parentOpacity / 255.0;

	if (_cascadeOpacityEnabled)
	{
		Object* pObj;
		ARRAY_FOREACH(_pChildren, pObj)
		{
			RGBAProtocol* item = dynamic_cast<RGBAProtocol*>(pObj);
			if (item)
			{
				item->updateDisplayedOpacity(_displayedOpacity);
			}
		}
	}
}





NS_CC_END


