#include "LayerColor.h"
#include "Director.h"
#include "shaders/GLProgram.h"
#include "shaders/GLStateCache.h"
#include "shaders/ShaderCache.h"

#ifdef EMSCRIPTEN
#include "nodes/base_nodes/GLBufferedNode.h"
#endif



NS_CC_BEGIN


LayerColor* LayerColor::create()
{
	LayerColor* pRet = new LayerColor();
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

LayerColor* LayerColor::create(const ccColor4B& color, GLfloat width, GLfloat height)
{
	LayerColor* pLayer = new LayerColor();
	if (pLayer && pLayer->initWithColor(color, width, height))
	{
		pLayer->autorelease();
		return pLayer;
	}
	CC_SAFE_DELETE(pLayer);
	return NULL;
}

LayerColor* LayerColor::create(const ccColor4B& color)
{
	LayerColor* pLayer = new LayerColor();
	if (pLayer && pLayer->initWithColor(color))
	{
		pLayer->autorelease();
		return pLayer;
	}
	CC_SAFE_DELETE(pLayer);
	return NULL;
}

LayerColor::LayerColor()
{
	_tBlendFunc.src = CC_BLEND_SRC;
	_tBlendFunc.dst = CC_BLEND_DST;
}

LayerColor::~LayerColor()
{
}

bool LayerColor::init()
{
	Size s = Director::sharedDirector()->getWinSize();
	return initWithColor(ccc4(0, 0, 0, 0), s.width, s.height);
}


bool LayerColor::initWithColor(const ccColor4B& color, GLfloat w, GLfloat h)
{
	if (Layer::init())
	{
		_tBlendFunc.src = GL_SRC_ALPHA;
		_tBlendFunc.dst = GL_ONE_MINUS_SRC_ALPHA;

		_displayedColor.r = _realColor.r = color.r;
		_displayedColor.g = _realColor.g = color.g;
		_displayedColor.b = _realColor.b = color.b;
		_displayedOpacity = _realOpacity = color.a;

		for (size_t i = 0; i < sizeof(_pSquareVertices) / sizeof(_pSquareVertices[0]); i++)
		{
			_pSquareVertices[i].x = 0.0f;
			_pSquareVertices[i].y = 0.0f;
		}

		updateColor();
		setContentSize(SizeMake(w, h));

		setShaderProgram(ShaderCache::sharedShaderCache()->programForKey(kCCShader_PositionColor));
	}
	return true;
}

bool LayerColor::initWithColor(const ccColor4B& color)
{
	Size s = Director::sharedDirector()->getWinSize();
	this->initWithColor(color, s.width, s.height);
	return true;
}

void LayerColor::updateColor()
{
	for (unsigned int i = 0; i < 4; i++)
	{
		_pSquareColors[i].r = _displayedColor.r / 255.0f;
		_pSquareColors[i].g = _displayedColor.g / 255.0f;
		_pSquareColors[i].b = _displayedColor.b / 255.0f;
		_pSquareColors[i].a = _displayedOpacity / 255.0f;
	}
}


void LayerColor::changeWidth(GLfloat w)
{
	this->setContentSize(SizeMake(w, _obContentSize.height));
}

void LayerColor::changeHeight(GLfloat h)
{
	this->setContentSize(SizeMake(_obContentSize.width, h));
}

void LayerColor::changeWidthAndHeight(GLfloat w, GLfloat h)
{
	this->setContentSize(SizeMake(w, h));
}


void LayerColor::setColor(const ccColor3B& color)
{
	LayerRGBA::setColor(color);
	updateColor();
}

void LayerColor::setOpacity(GLubyte opacity)
{
	LayerRGBA::setOpacity(opacity);
	updateColor();
}

ccBlendFunc LayerColor::getBlendFunc()
{
	return _tBlendFunc;
}

void LayerColor::setBlendFunc(ccBlendFunc var)
{
	_tBlendFunc = var;
}

// 四个顶点的排序?
// 2   3
// 0   1
void LayerColor::setContentSize(const Size& size)
{
	_pSquareVertices[1].x = size.width;
	_pSquareVertices[2].y = size.height;
	_pSquareVertices[3].x = size.width;
	_pSquareVertices[3].y = size.height;

	Layer::setContentSize(size);
}


void LayerColor::draw()
{
	CC_NODE_DRAW_SETUP();

	ccGLEnableVertexAttribs(kCCVertexAttribFlag_Position | kCCVertexAttribFlag_Color);

	//
	// Attributes
	//
#ifdef EMSCRIPTEN
	setGLBufferData(_pSquareVertices, 4 * sizeof(ccVertex2F), 0);
	glVertexAttribPointer(kCCVertexAttrib_Position, 2, GL_FLOAT, GL_FALSE, 0, 0);

	setGLBufferData(_pSquareColors, 4 * sizeof(ccColor4F), 1);
	glVertexAttribPointer(kCCVertexAttrib_Color, 4, GL_FLOAT, GL_FALSE, 0, 0);
#else
	glVertexAttribPointer(kCCVertexAttrib_Position, 2, GL_FLOAT, GL_FALSE, 0, _pSquareVertices);
	glVertexAttribPointer(kCCVertexAttrib_Color, 4, GL_FLOAT, GL_FALSE, 0, _pSquareColors);
#endif // EMSCRIPTEN

	ccGLBlendFunc(_tBlendFunc.src, _tBlendFunc.dst);

	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	CC_INCREMENT_GL_DRAWS(1);
}



NS_CC_END