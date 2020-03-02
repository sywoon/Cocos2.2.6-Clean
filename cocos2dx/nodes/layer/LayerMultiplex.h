#pragma once

#include "ccMacros.h"
#include "Layer.h"


NS_CC_BEGIN


class CC_DLL LayerMultiplex : public Layer
{
public:
    LayerMultiplex();
    virtual ~LayerMultiplex();

    static LayerMultiplex* create();
    static LayerMultiplex* create(Layer* layer, ...);
    static LayerMultiplex* createWithArray(Array* arrayOfLayers);
    static LayerMultiplex* createWithLayer(Layer* layer);


protected:
    bool    initWithLayers(Layer* layer, va_list params);
    bool    initWithArray(Array* arrayOfLayers);

private:
    void    addLayer(Layer* layer);
    void    switchTo(unsigned int n);
    void    switchToAndReleaseMe(unsigned int n);


protected:
	unsigned int    _nEnabledLayer;
	Array*          _pLayers;
};



NS_CC_END
