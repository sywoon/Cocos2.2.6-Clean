#pragma once

#include "ccMacros.h"
#include "nodes/base_nodes/NodeRGBA.h"
#include "cocoa/Geometry.h"


NS_CC_BEGIN


class Texture2D;
class CC_DLL Sprite : public NodeRGBA, public TextureProtocol
{
public:
	Sprite(void);
	virtual ~Sprite(void);

	static Sprite* create();
	static Sprite* create(const char* pszFileName);
	static Sprite* create(const char* pszFileName, const Rect& rect);
	static Sprite* createWithTexture(Texture2D* pTexture);
	static Sprite* createWithTexture(Texture2D* pTexture, const Rect& rect);
	//static CCSprite* createWithSpriteFrame(CCSpriteFrame* pSpriteFrame);
	//static CCSprite* createWithSpriteFrameName(const char* pszSpriteFrameName);


protected:
	virtual bool init(void);
	virtual bool initWithTexture(Texture2D* pTexture);
	virtual bool initWithTexture(Texture2D* pTexture, const Rect& rect);
	virtual bool initWithTexture(Texture2D* pTexture, const Rect& rect, bool rotated);
	//virtual bool initWithSpriteFrame(CCSpriteFrame* pSpriteFrame);
	//virtual bool initWithSpriteFrameName(const char* pszSpriteFrameName);
	virtual bool initWithFile(const char* pszFilename);
	virtual bool initWithFile(const char* pszFilename, const Rect& rect);

public:
	virtual void		setTexture(Texture2D* texture);
	virtual Texture2D*	getTexture(void);
	inline void			setBlendFunc(ccBlendFunc blendFunc) { _sBlendFunc = blendFunc; }
	inline ccBlendFunc	getBlendFunc(void) { return _sBlendFunc; }

	inline virtual void setDirty(bool bDirty) { _bDirty = bDirty; }
	inline virtual bool isDirty(void) { return _bDirty; }

	inline ccV3F_C4B_T2F_Quad getQuad(void) { return _sQuad; }

	virtual void setTextureRect(const Rect& rect);
	virtual void setTextureRect(const Rect& rect, bool rotated, const Size& untrimmedSize);

	virtual bool isFlipX(void);
	virtual void setFlipX(bool bFlipX);
	virtual bool isFlipY(void);
	virtual void setFlipY(bool bFlipY);

	virtual void updateTransform(void);
	virtual void draw(void);


protected:
	virtual void setVertexRect(const Rect& rect);
	virtual void setTextureCoords(Rect rect);


public:
	// NodeRGBA  extend
	virtual void setOpacityModifyRGB(bool modify);
	virtual bool isOpacityModifyRGB(void);

	virtual void updateBlendFunc(void);
	

public:
	virtual void setPosition(const Point& pos);

	virtual void setScale(float fScale);
	virtual void setScaleX(float fScaleX);
	virtual void setScaleY(float fScaleY);
	
	virtual void setRotation(float fRotation);
	virtual void setRotationX(float fRotationX);
	virtual void setRotationY(float fRotationY);

	virtual void setSkewX(float sx);
	virtual void setSkewY(float sy);

	virtual void setVertexZ(float fVertexZ);
	virtual void setAnchorPoint(const Point& anchor);
	virtual void setVisible(bool bVisible);
	

protected:
	virtual void setDirtyRecursively(bool bValue);



protected:
	void		updateColor(void);
	



private:
	ccBlendFunc     _sBlendFunc;
	Texture2D*		_pobTexture;

	ccV3F_C4B_T2F_Quad  _sQuad;

	// texture
	Rect			_obRect;			 /// Retangle of CCTexture2D
	bool			_bRectRotated;

	bool            _bDirty;            /// Whether the sprite needs to be updated
	bool            _bRecursiveDirty;   /// Whether all of the sprite's children needs to be updated
	bool			_bOpacityModifyRGB; /// opacity and RGB protocol
	bool            _bShouldBeHidden;   /// should not be drawn because one of the ancestors is not visible

	bool			_bFlipX;            /// Whether the sprite is flipped horizaontally or not.
	bool			_bFlipY;            /// Whether the sprite is flipped vertically or not.

	Point			_obOffsetPosition;
	Point			_obUnflippedOffsetPositionFromCenter;

	bool            _bHasChildren;
};



NS_CC_END
