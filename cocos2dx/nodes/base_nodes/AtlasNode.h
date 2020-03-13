#pragma once


#include "ccMacros.h"
#include "NodeRGBA.h"


NS_CC_BEGIN


class TextureAtlas;
class CC_DLL CCAtlasNode : public NodeRGBA, public TextureProtocol
{
public:
    CCAtlasNode();
    virtual ~CCAtlasNode();

	static CCAtlasNode* create(const char* tile, unsigned int tileWidth, 
								unsigned int tileHeight, unsigned int itemsToRender);

public:
	virtual void	updateAtlasValues();
	virtual void	draw(void);

private:
	bool	initWithTileFile(const char* tile, unsigned int tileWidth, unsigned int tileHeight, unsigned int itemsToRender);
	bool	initWithTexture(Texture2D* texture, unsigned int tileWidth, unsigned int tileHeight, unsigned int itemsToRender);

	
public:
	virtual Texture2D*  getTexture(void);
	virtual void		setTexture(Texture2D* texture);

	virtual const ccColor3B& getColor(void);
	virtual void		setColor(const ccColor3B& color);

	virtual bool		isOpacityModifyRGB();
	virtual void		setOpacityModifyRGB(bool isOpacityModifyRGB);
	
	virtual void		setOpacity(GLubyte opacity);

private:
	void		calculateMaxItems();
	void		updateBlendFunc();
	void		updateOpacityModifyRGB();



private:
	CC_PROPERTY(TextureAtlas*, m_pTextureAtlas, TextureAtlas);
	CC_PROPERTY(ccBlendFunc, m_tBlendFunc, BlendFunc);
	CC_PROPERTY(unsigned int, m_uQuadsToDraw, QuadsToDraw);

	GLint    m_nUniformColor;

	//! chars per row
	unsigned int m_uItemsPerRow;
	unsigned int m_uItemsPerColumn;

	//! width of each char
	unsigned int    m_uItemWidth;
	unsigned int    m_uItemHeight;

	ccColor3B    m_tColorUnmodified;

	bool m_bIsOpacityModifyRGB;
};





NS_CC_END