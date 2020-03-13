#include "AtlasNode.h"
#include "textures/TextureAtlas.h"
#include "textures/TextureCache.h"
#include "textures/Texture2D.h"
#include "shaders/GLProgram.h"
#include "shaders/GLStateCache.h"



NS_CC_BEGIN

CCAtlasNode* CCAtlasNode::create(const char* tile, unsigned int tileWidth, 
							unsigned int tileHeight, unsigned int itemsToRender)
{
	CCAtlasNode* pRet = new CCAtlasNode();
	if (pRet->initWithTileFile(tile, tileWidth, tileHeight, itemsToRender))
	{
		pRet->autorelease();
		return pRet;
	}
	CC_SAFE_DELETE(pRet);
	return NULL;
}

CCAtlasNode::CCAtlasNode()
	: m_uItemsPerRow(0)
	, m_uItemsPerColumn(0)
	, m_uItemWidth(0)
	, m_uItemHeight(0)
	, m_pTextureAtlas(NULL)
	, m_bIsOpacityModifyRGB(false)
	, m_uQuadsToDraw(0)
	, m_nUniformColor(0)
{
}

CCAtlasNode::~CCAtlasNode()
{
	CC_SAFE_RELEASE(m_pTextureAtlas);
}



bool CCAtlasNode::initWithTileFile(const char* tile, unsigned int tileWidth, unsigned int tileHeight, 
						unsigned int itemsToRender)
{
	CCAssert(tile != NULL, "title should not be null");
	Texture2D* texture = TextureCache::sharedTextureCache()->addImage(tile);
	return initWithTexture(texture, tileWidth, tileHeight, itemsToRender);
}

bool CCAtlasNode::initWithTexture(Texture2D* texture, unsigned int tileWidth, unsigned int tileHeight,
						unsigned int itemsToRender)
{
	m_uItemWidth = tileWidth;
	m_uItemHeight = tileHeight;

	m_tColorUnmodified = ccWHITE;
	m_bIsOpacityModifyRGB = true;

	m_tBlendFunc.src = CC_BLEND_SRC;
	m_tBlendFunc.dst = CC_BLEND_DST;

	m_pTextureAtlas = new TextureAtlas();
	m_pTextureAtlas->initWithTexture(texture, itemsToRender);

	if (!m_pTextureAtlas)
	{
		CCLOG("cocos2d: Could not initialize CCAtlasNode. Invalid Texture.");
		return false;
	}

	this->updateBlendFunc();
	this->updateOpacityModifyRGB();

	this->calculateMaxItems();

	m_uQuadsToDraw = itemsToRender;

	setShaderProgram(ShaderCache::sharedShaderCache()->programForKey(kCCShader_PositionTexture_uColor));
	m_nUniformColor = glGetUniformLocation(getShaderProgram()->getProgram(), "u_color");

	return true;
}

void CCAtlasNode::updateAtlasValues()
{
	CCAssert(false, "CCAtlasNode:Abstract updateAtlasValue not overridden");
}

void CCAtlasNode::draw(void)
{
	CC_NODE_DRAW_SETUP();

	ccGLBlendFunc(m_tBlendFunc.src, m_tBlendFunc.dst);

	GLfloat colors[4] = { _displayedColor.r / 255.0f, _displayedColor.g / 255.0f, _displayedColor.b / 255.0f, _displayedOpacity / 255.0f };
	getShaderProgram()->setUniformLocationWith4fv(m_nUniformColor, colors, 1);

	m_pTextureAtlas->drawNumberOfQuads(m_uQuadsToDraw, 0);
}

Texture2D* CCAtlasNode::getTexture()
{
	return m_pTextureAtlas->getTexture();
}

void CCAtlasNode::setTexture(Texture2D* texture)
{
	m_pTextureAtlas->setTexture(texture);
	this->updateBlendFunc();
	this->updateOpacityModifyRGB();
}


const ccColor3B& CCAtlasNode::getColor()
{
	if (m_bIsOpacityModifyRGB)
	{
		return m_tColorUnmodified;
	}
	return NodeRGBA::getColor();
}

void CCAtlasNode::setColor(const ccColor3B& color3)
{
	ccColor3B tmp = color3;
	m_tColorUnmodified = color3;

	if (m_bIsOpacityModifyRGB)
	{
		tmp.r = tmp.r * _displayedOpacity / 255;
		tmp.g = tmp.g * _displayedOpacity / 255;
		tmp.b = tmp.b * _displayedOpacity / 255;
	}
	NodeRGBA::setColor(tmp);
}

bool CCAtlasNode::isOpacityModifyRGB()
{
	return m_bIsOpacityModifyRGB;
}

void CCAtlasNode::setOpacityModifyRGB(bool bValue)
{
	ccColor3B oldColor = this->getColor();
	m_bIsOpacityModifyRGB = bValue;
	this->setColor(oldColor);
}


void CCAtlasNode::updateOpacityModifyRGB()
{
	m_bIsOpacityModifyRGB = m_pTextureAtlas->getTexture()->hasPremultipliedAlpha();
}

void CCAtlasNode::setOpacity(GLubyte opacity)
{
	NodeRGBA::setOpacity(opacity);

	// special opacity for premultiplied textures
	if (m_bIsOpacityModifyRGB)
		this->setColor(m_tColorUnmodified);
}

void CCAtlasNode::calculateMaxItems()
{
	Size s = m_pTextureAtlas->getTexture()->getContentSize();

	m_uItemsPerColumn = (int)(s.height / m_uItemHeight);
	m_uItemsPerRow = (int)(s.width / m_uItemWidth);
}


void CCAtlasNode::updateBlendFunc()
{
	if (!m_pTextureAtlas->getTexture()->hasPremultipliedAlpha()) 
	{
		m_tBlendFunc.src = GL_SRC_ALPHA;
		m_tBlendFunc.dst = GL_ONE_MINUS_SRC_ALPHA;
	}
}

void CCAtlasNode::setTextureAtlas(TextureAtlas* var)
{
	CC_SAFE_RETAIN(var);
	CC_SAFE_RELEASE(m_pTextureAtlas);
	m_pTextureAtlas = var;
}

TextureAtlas* CCAtlasNode::getTextureAtlas()
{
	return m_pTextureAtlas;
}

ccBlendFunc CCAtlasNode::getBlendFunc()
{
	return m_tBlendFunc;
}

void CCAtlasNode::setBlendFunc(ccBlendFunc blendFunc)
{
	m_tBlendFunc = blendFunc;
}

unsigned int CCAtlasNode::getQuadsToDraw()
{
	return m_uQuadsToDraw;
}

void CCAtlasNode::setQuadsToDraw(unsigned int uQuadsToDraw)
{
	m_uQuadsToDraw = uQuadsToDraw;
}








NS_CC_END


