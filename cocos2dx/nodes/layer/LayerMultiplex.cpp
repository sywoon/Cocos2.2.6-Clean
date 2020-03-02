#include "LayerMultiplex.h"

NS_CC_BEGIN


LayerMultiplex* LayerMultiplex::create()
{
	LayerMultiplex* pRet = new LayerMultiplex();
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

LayerMultiplex* LayerMultiplex::create(Layer* layer, ...)
{
	va_list args;
	va_start(args, layer);

	LayerMultiplex* pMultiplexLayer = new LayerMultiplex();
	if (pMultiplexLayer && pMultiplexLayer->initWithLayers(layer, args))
	{
		pMultiplexLayer->autorelease();
		va_end(args);
		return pMultiplexLayer;
	}
	va_end(args);
	CC_SAFE_DELETE(pMultiplexLayer);
	return NULL;
}

LayerMultiplex* LayerMultiplex::createWithLayer(Layer* layer)
{
	return LayerMultiplex::create(layer, NULL);
}

LayerMultiplex* LayerMultiplex::createWithArray(Array* arrayOfLayers)
{
	LayerMultiplex* pRet = new LayerMultiplex();
	if (pRet && pRet->initWithArray(arrayOfLayers))
	{
		pRet->autorelease();
	}
	else
	{
		CC_SAFE_DELETE(pRet);
	}
	return pRet;
}


LayerMultiplex::LayerMultiplex()
	: _nEnabledLayer(0)
	, _pLayers(NULL)
{
}
LayerMultiplex::~LayerMultiplex()
{
	CC_SAFE_RELEASE(_pLayers);
}



bool LayerMultiplex::initWithLayers(Layer* layer, va_list params)
{
	if (Layer::init())
	{
		_pLayers = Array::createWithCapacity(5);
		_pLayers->retain();
		_pLayers->addObject(layer);

		Layer* l = va_arg(params, Layer*);
		while (l) {
			_pLayers->addObject(l);
			l = va_arg(params, Layer*);
		}

		_nEnabledLayer = 0;
		this->addChild((Node*)_pLayers->objectAtIndex(_nEnabledLayer));
		return true;
	}

	return false;
}

bool LayerMultiplex::initWithArray(Array* arrayOfLayers)
{
	if (Layer::init())
	{
		_pLayers = Array::createWithCapacity(arrayOfLayers->count());
		_pLayers->addObjectsFromArray(arrayOfLayers);
		_pLayers->retain();

		_nEnabledLayer = 0;
		this->addChild((Node*)_pLayers->objectAtIndex(_nEnabledLayer));
		return true;
	}
	return false;
}

void LayerMultiplex::addLayer(Layer* layer)
{
	CCAssert(_pLayers, "");
	_pLayers->addObject(layer);
}


void LayerMultiplex::switchTo(unsigned int n)
{
	CCAssert(n < _pLayers->count(), "Invalid index in MultiplexLayer switchTo message");

	this->removeChild((Node*)_pLayers->objectAtIndex(_nEnabledLayer), true);

	_nEnabledLayer = n;

	this->addChild((Node*)_pLayers->objectAtIndex(n));
}

void LayerMultiplex::switchToAndReleaseMe(unsigned int n)
{
	CCAssert(n < _pLayers->count(), "Invalid index in MultiplexLayer switchTo message");

	this->removeChild((Node*)_pLayers->objectAtIndex(_nEnabledLayer), true);

	this->addChild((Node*)_pLayers->objectAtIndex(n));

	_pLayers->removeObjectAtIndex(_nEnabledLayer, true);
	_nEnabledLayer = n;
}







NS_CC_END