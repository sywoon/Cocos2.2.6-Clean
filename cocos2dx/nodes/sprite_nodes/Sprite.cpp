#include "Sprite.h"
#include "textures/Texture2D.h"
#include "textures/TextureCache.h"
#include "shaders/ShaderCache.h"
#include "shaders/GLProgram.h"
#include "shaders/GLStateCache.h"
#include "Image.h"


NS_CC_BEGIN

#if CC_SPRITEBATCHNODE_RENDER_SUBPIXEL
#define RENDER_IN_SUBPIXEL
#else
#define RENDER_IN_SUBPIXEL(__ARGS__) (ceil(__ARGS__))
#endif



Sprite::Sprite(void)
	: _pobTexture(NULL)
	, _bRectRotated(false)
	, _bDirty(false)
	, _bRecursiveDirty(false)
	, _bOpacityModifyRGB(false)
	, _bShouldBeHidden(false)
	, _bFlipX(false)
	, _bFlipY(false)
	, _bHasChildren(false)
{
}

Sprite::~Sprite(void)
{
	CC_SAFE_RELEASE(_pobTexture);
}


//////////////////////////////////
// 创建和初始化
//

Sprite* Sprite::create()
{
	Sprite* pSprite = new Sprite();
	if (pSprite && pSprite->init())
	{
		pSprite->autorelease();
		return pSprite;
	}
	CC_SAFE_DELETE(pSprite);
	return NULL;
}

Sprite* Sprite::create(const char* pszFileName)
{
	Sprite* pobSprite = new Sprite();
	if (pobSprite && pobSprite->initWithFile(pszFileName))
	{
		pobSprite->autorelease();
		return pobSprite;
	}
	CC_SAFE_DELETE(pobSprite);
	return NULL;
}

Sprite* Sprite::create(const char* pszFileName, const Rect& rect)
{
	Sprite* pobSprite = new Sprite();
	if (pobSprite && pobSprite->initWithFile(pszFileName, rect))
	{
		pobSprite->autorelease();
		return pobSprite;
	}
	CC_SAFE_DELETE(pobSprite);
	return NULL;
}

Sprite* Sprite::createWithTexture(Texture2D* pTexture)
{
	Sprite* pobSprite = new Sprite();
	if (pobSprite && pobSprite->initWithTexture(pTexture))
	{
		pobSprite->autorelease();
		return pobSprite;
	}
	CC_SAFE_DELETE(pobSprite);
	return NULL;
}

Sprite* Sprite::createWithTexture(Texture2D* pTexture, const Rect& rect)
{
	Sprite* pobSprite = new Sprite();
	if (pobSprite && pobSprite->initWithTexture(pTexture, rect))
	{
		pobSprite->autorelease();
		return pobSprite;
	}
	CC_SAFE_DELETE(pobSprite);
	return NULL;
}

bool Sprite::init(void)
{
	return initWithTexture(NULL, RectZero);
}

bool Sprite::initWithTexture(Texture2D* pTexture)
{
	CCAssert(pTexture != NULL, "Invalid texture for sprite");

	Rect rect = RectZero;
	rect.size = pTexture->getContentSize();

	return initWithTexture(pTexture, rect);
}

bool Sprite::initWithTexture(Texture2D* pTexture, const Rect& rect)
{
	return initWithTexture(pTexture, rect, false);
}

bool Sprite::initWithTexture(Texture2D* pTexture, const Rect& rect, bool rotated)
{
	if (!NodeRGBA::init())
		return false;

	_sBlendFunc.src = CC_BLEND_SRC;
	_sBlendFunc.dst = CC_BLEND_DST;

	_obOffsetPosition = PointZero;

	_bHasChildren = false;
	_bRecursiveDirty = false;
	_bFlipX = _bFlipY = false;
	_bOpacityModifyRGB = true;

	memset(&_sQuad, 0, sizeof(_sQuad));

	// Atlas: Color
	ccColor4B tmpColor = { 255, 255, 255, 255 };
	_sQuad.bl.colors = tmpColor;
	_sQuad.br.colors = tmpColor;
	_sQuad.tl.colors = tmpColor;
	_sQuad.tr.colors = tmpColor;

	setShaderProgram(ShaderCache::sharedShaderCache()->programForKey(kCCShader_PositionTextureColor));

	setTexture(pTexture);
	setTextureRect(rect, rotated, rect.size);

	setAnchorPoint(ccp(0.5f, 0.5f));
	setDirty(false);

	return true;
}

bool Sprite::initWithFile(const char* pszFilename)
{
	CCAssert(pszFilename != NULL, "Invalid filename for sprite");

	Texture2D* pTexture = TextureCache::sharedTextureCache()->addImage(pszFilename);
	if (pTexture)
	{
		Rect rect = RectZero;
		rect.size = pTexture->getContentSize();
		return initWithTexture(pTexture, rect);
	}

	// don't release here.
	// when load texture failed, it's better to get a "transparent" sprite than a crashed program
	// this->release(); 
	return false;
}

bool Sprite::initWithFile(const char* pszFilename, const Rect& rect)
{
	CCAssert(pszFilename != NULL, "");

	Texture2D* pTexture = TextureCache::sharedTextureCache()->addImage(pszFilename);
	if (pTexture)
	{
		return initWithTexture(pTexture, rect);
	}

	// don't release here.
	// when load texture failed, it's better to get a "transparent" sprite than a crashed program
	// this->release(); 
	return false;
}




//////////////////////////////////
// NodeRGBA  extend
//
void Sprite::setOpacityModifyRGB(bool modify)
{
	if (_bOpacityModifyRGB != modify)
	{
		_bOpacityModifyRGB = modify;
		updateColor();
	}
}

bool Sprite::isOpacityModifyRGB(void)
{
	return _bOpacityModifyRGB;
}

void Sprite::updateBlendFunc(void)
{
	// it is possible to have an untextured sprite
	if (!_pobTexture || !_pobTexture->hasPremultipliedAlpha())
	{
		_sBlendFunc.src = GL_SRC_ALPHA;
		_sBlendFunc.dst = GL_ONE_MINUS_SRC_ALPHA;
		setOpacityModifyRGB(false);
	}
	else
	{
		_sBlendFunc.src = CC_BLEND_SRC;
		_sBlendFunc.dst = CC_BLEND_DST;
		setOpacityModifyRGB(true);
	}
}

void Sprite::updateColor(void)
{
	ccColor4B color4 = { _displayedColor.r, _displayedColor.g, _displayedColor.b, _displayedOpacity };

	// special opacity for premultiplied textures
	if (_bOpacityModifyRGB)
	{
		color4.r *= _displayedOpacity / 255.0f;
		color4.g *= _displayedOpacity / 255.0f;
		color4.b *= _displayedOpacity / 255.0f;
	}

	_sQuad.bl.colors = color4;
	_sQuad.br.colors = color4;
	_sQuad.tl.colors = color4;
	_sQuad.tr.colors = color4;
}



static unsigned char cc_2x2_white_image[] = {
	// RGBA8888
	0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF
};

#define CC_2x2_WHITE_IMAGE_KEY  "cc_2x2_white_image"
void Sprite::setTexture(Texture2D* texture)
{
	// accept texture==nil as argument
	CCAssert(!texture || dynamic_cast<Texture2D*>(texture), "setTexture expects a Texture2D. Invalid argument");

	if (NULL == texture)
	{
		// Gets the texture by key firstly.
		texture = TextureCache::sharedTextureCache()->textureForKey(CC_2x2_WHITE_IMAGE_KEY);

		// If texture wasn't in cache, create it from RAW data.
		if (NULL == texture)
		{
			Image* image = new Image();
			bool isOK = image->initWithImageData(cc_2x2_white_image, sizeof(cc_2x2_white_image), Image::kFmtRawData, 2, 2, 8);
			CCAssert(isOK, "The 2x2 empty texture was created unsuccessfully.");

			texture = TextureCache::sharedTextureCache()->addUIImage(image, CC_2x2_WHITE_IMAGE_KEY);
			CC_SAFE_RELEASE(image);
		}
	}

	if (_pobTexture != texture)
	{
		CC_SAFE_RETAIN(texture);
		CC_SAFE_RELEASE(_pobTexture);
		_pobTexture = texture;
		updateBlendFunc();
	}
}

Texture2D* Sprite::getTexture(void)
{
	return _pobTexture;
}


void Sprite::setTextureRect(const Rect& rect)
{
	setTextureRect(rect, false, rect.size);
}


void Sprite::setTextureRect(const Rect& rect, bool rotated, const Size& untrimmedSize)
{
	_bRectRotated = rotated;

	setContentSize(untrimmedSize);
	setVertexRect(rect);
	setTextureCoords(rect);

	Point relativeOffset = _obUnflippedOffsetPositionFromCenter;

	if (_bFlipX)
	{
		relativeOffset.x = -relativeOffset.x;
	}
	if (_bFlipY)
	{
		relativeOffset.y = -relativeOffset.y;
	}

	_obOffsetPosition.x = relativeOffset.x + (_obContentSize.width - _obRect.size.width) / 2;
	_obOffsetPosition.y = relativeOffset.y + (_obContentSize.height - _obRect.size.height) / 2;

	// Atlas: Vertex
	float x1 = 0 + _obOffsetPosition.x;
	float y1 = 0 + _obOffsetPosition.y;
	float x2 = x1 + _obRect.size.width;
	float y2 = y1 + _obRect.size.height;

	// Don't update Z.
	_sQuad.bl.vertices = vertex3(x1, y1, 0);
	_sQuad.br.vertices = vertex3(x2, y1, 0);
	_sQuad.tl.vertices = vertex3(x1, y2, 0);
	_sQuad.tr.vertices = vertex3(x2, y2, 0);
}

void Sprite::setVertexRect(const Rect& rect)
{
	_obRect = rect;
}


void Sprite::setTextureCoords(Rect rect)
{
	Texture2D* tex = _pobTexture;
	if (!tex)
		return;

	float atlasWidth = (float)tex->getPixelsWide();
	float atlasHeight = (float)tex->getPixelsHigh();

	float left, right, top, bottom;

	if (_bRectRotated)
	{
#if CC_FIX_ARTIFACTS_BY_STRECHING_TEXEL
		left = (2 * rect.origin.x + 1) / (2 * atlasWidth);
		right = left + (rect.size.height * 2 - 2) / (2 * atlasWidth);
		top = (2 * rect.origin.y + 1) / (2 * atlasHeight);
		bottom = top + (rect.size.width * 2 - 2) / (2 * atlasHeight);
#else
		left = rect.origin.x / atlasWidth;
		right = (rect.origin.x + rect.size.height) / atlasWidth;
		top = rect.origin.y / atlasHeight;
		bottom = (rect.origin.y + rect.size.width) / atlasHeight;
#endif

		if (_bFlipX)
		{
			CC_SWAP(top, bottom, float);
		}

		if (_bFlipY)
		{
			CC_SWAP(left, right, float);
		}

		_sQuad.bl.texCoords.u = left;
		_sQuad.bl.texCoords.v = top;
		_sQuad.br.texCoords.u = left;
		_sQuad.br.texCoords.v = bottom;
		_sQuad.tl.texCoords.u = right;
		_sQuad.tl.texCoords.v = top;
		_sQuad.tr.texCoords.u = right;
		_sQuad.tr.texCoords.v = bottom;
	}
	else
	{
#if CC_FIX_ARTIFACTS_BY_STRECHING_TEXEL
		left = (2 * rect.origin.x + 1) / (2 * atlasWidth);
		right = left + (rect.size.width * 2 - 2) / (2 * atlasWidth);
		top = (2 * rect.origin.y + 1) / (2 * atlasHeight);
		bottom = top + (rect.size.height * 2 - 2) / (2 * atlasHeight);
#else
		left = rect.origin.x / atlasWidth;
		right = (rect.origin.x + rect.size.width) / atlasWidth;
		top = rect.origin.y / atlasHeight;
		bottom = (rect.origin.y + rect.size.height) / atlasHeight;
#endif

		if (_bFlipX)
		{
			CC_SWAP(left, right, float);
		}

		if (_bFlipY)
		{
			CC_SWAP(top, bottom, float);
		}

		_sQuad.bl.texCoords.u = left;
		_sQuad.bl.texCoords.v = bottom;
		_sQuad.br.texCoords.u = right;
		_sQuad.br.texCoords.v = bottom;
		_sQuad.tl.texCoords.u = left;
		_sQuad.tl.texCoords.v = top;
		_sQuad.tr.texCoords.u = right;
		_sQuad.tr.texCoords.v = top;
	}
}


void Sprite::setFlipX(bool bFlipX)
{
	if (_bFlipX != bFlipX)
	{
		_bFlipX = bFlipX;
		setTextureRect(_obRect, _bRectRotated, _obContentSize);
	}
}

bool Sprite::isFlipX(void)
{
	return _bFlipX;
}

void Sprite::setFlipY(bool bFlipY)
{
	if (_bFlipY != bFlipY)
	{
		_bFlipY = bFlipY;
		setTextureRect(_obRect, _bRectRotated, _obContentSize);
	}
}

bool Sprite::isFlipY(void)
{
	return _bFlipY;
}


void Sprite::updateTransform(void)
{
	// recalculate matrix only if it is dirty
	if (isDirty()) 
	{
		// If it is not visible, or one of its ancestors is not visible, then do nothing:
		if (!_bVisible || (_pParent && ((Sprite*)_pParent)->_bShouldBeHidden))
		{
			_sQuad.br.vertices = _sQuad.tl.vertices = _sQuad.tr.vertices = _sQuad.bl.vertices = vertex3(0, 0, 0);
			_bShouldBeHidden = true;
		}
		else
		{
			_bShouldBeHidden = false;
			
			Size size = _obRect.size;
			AffineTransform transform = nodeToParentTransform();

			// calculate the Quad based on the Affine Matrix
			float x1 = _obOffsetPosition.x;
			float y1 = _obOffsetPosition.y;

			float x2 = x1 + size.width;
			float y2 = y1 + size.height;
			float x = transform.tx;
			float y = transform.ty;

			float cr = transform.a;
			float sr = transform.b;
			float cr2 = transform.d;
			float sr2 = -transform.c;
			float ax = x1 * cr - y1 * sr2 + x;
			float ay = x1 * sr + y1 * cr2 + y;

			float bx = x2 * cr - y1 * sr2 + x;
			float by = x2 * sr + y1 * cr2 + y;

			float cx = x2 * cr - y2 * sr2 + x;
			float cy = x2 * sr + y2 * cr2 + y;

			float dx = x1 * cr - y2 * sr2 + x;
			float dy = x1 * sr + y2 * cr2 + y;

			_sQuad.bl.vertices = vertex3(RENDER_IN_SUBPIXEL(ax), RENDER_IN_SUBPIXEL(ay), _fVertexZ);
			_sQuad.br.vertices = vertex3(RENDER_IN_SUBPIXEL(bx), RENDER_IN_SUBPIXEL(by), _fVertexZ);
			_sQuad.tl.vertices = vertex3(RENDER_IN_SUBPIXEL(dx), RENDER_IN_SUBPIXEL(dy), _fVertexZ);
			_sQuad.tr.vertices = vertex3(RENDER_IN_SUBPIXEL(cx), RENDER_IN_SUBPIXEL(cy), _fVertexZ);
		}

		_bRecursiveDirty = false;
		setDirty(false);
	}

	Node::updateTransform();

#if CC_SPRITE_DEBUG_DRAW
	// draw bounding box
	CCPoint vertices[4] = {
		ccp(_sQuad.bl.vertices.x, _sQuad.bl.vertices.y),
		ccp(_sQuad.br.vertices.x, _sQuad.br.vertices.y),
		ccp(_sQuad.tr.vertices.x, _sQuad.tr.vertices.y),
		ccp(_sQuad.tl.vertices.x, _sQuad.tl.vertices.y),
	};
	ccDrawPoly(vertices, 4, true);
#endif // CC_SPRITE_DEBUG_DRAW
}

// draw

void Sprite::draw(void)
{
	CC_PROFILER_START_CATEGORY(kCCProfilerCategorySprite, "Sprite - draw");

	CC_NODE_DRAW_SETUP();

	ccGLBlendFunc(_sBlendFunc.src, _sBlendFunc.dst);

	ccGLBindTexture2D(_pobTexture->getName());
	ccGLEnableVertexAttribs(kCCVertexAttribFlag_PosColorTex);

#define kQuadSize sizeof(_sQuad.bl)
#ifdef EMSCRIPTEN
	long offset = 0;
	setGLBufferData(&_sQuad, 4 * kQuadSize, 0);
#else
	long offset = (long)&_sQuad;
#endif

	// vertex
	int diff = offsetof(ccV3F_C4B_T2F, vertices);
	glVertexAttribPointer(kCCVertexAttrib_Position, 3, GL_FLOAT, GL_FALSE, kQuadSize, (void*)(offset + diff));

	// texCoods
	diff = offsetof(ccV3F_C4B_T2F, texCoords);
	glVertexAttribPointer(kCCVertexAttrib_TexCoords, 2, GL_FLOAT, GL_FALSE, kQuadSize, (void*)(offset + diff));

	// color
	diff = offsetof(ccV3F_C4B_T2F, colors);
	glVertexAttribPointer(kCCVertexAttrib_Color, 4, GL_UNSIGNED_BYTE, GL_TRUE, kQuadSize, (void*)(offset + diff));

	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	CHECK_GL_ERROR_DEBUG();


#if CC_SPRITE_DEBUG_DRAW == 1
	// draw bounding box
	Point vertices[4] = {
		ccp(_sQuad.tl.vertices.x,_sQuad.tl.vertices.y),
		ccp(_sQuad.bl.vertices.x,_sQuad.bl.vertices.y),
		ccp(_sQuad.br.vertices.x,_sQuad.br.vertices.y),
		ccp(_sQuad.tr.vertices.x,_sQuad.tr.vertices.y),
	};
	ccDrawPoly(vertices, 4, true);
#elif CC_SPRITE_DEBUG_DRAW == 2
	// draw texture box
	Size s = this->getTextureRect().size;
	Point offsetPix = this->getOffsetPosition();
	Point vertices[4] = {
		ccp(offsetPix.x,offsetPix.y), ccp(offsetPix.x + s.width,offsetPix.y),
		ccp(offsetPix.x + s.width,offsetPix.y + s.height), ccp(offsetPix.x,offsetPix.y + s.height)
	};
	ccDrawPoly(vertices, 4, true);
#endif // CC_SPRITE_DEBUG_DRAW

	CC_INCREMENT_GL_DRAWS(1);

	CC_PROFILER_STOP_CATEGORY(kCCProfilerCategorySprite, "Sprite - draw");
}




//////////////////////////////////////////
// batchnode 需要 重写一些基本函数
//
void Sprite::setDirtyRecursively(bool bValue)
{
	_bRecursiveDirty = bValue;
	setDirty(bValue);
	// recursively set dirty
	if (_bHasChildren)
	{
		Object* pObject = NULL;
		ARRAY_FOREACH(_pChildren, pObject)
		{
			Sprite* pChild = dynamic_cast<Sprite*>(pObject);
			if (pChild)
			{
				pChild->setDirtyRecursively(true);
			}
		}
	}
}

#define SET_DIRTY_RECURSIVELY() {                                    \
                    if (/*m_pobBatchNode && */!_bRecursiveDirty) {      \
                        _bRecursiveDirty = true;                    \
                        setDirty(true);                              \
                        if ( _bHasChildren)                         \
                            setDirtyRecursively(true);               \
                    }												 \
                 }



void Sprite::setPosition(const Point& pos)
{
	Node::setPosition(pos);
	SET_DIRTY_RECURSIVELY();
}

void Sprite::setPosition(float x, float y)
{
	Node::setPosition(x, y);
	SET_DIRTY_RECURSIVELY();
}

void Sprite::setScale(float fScale)
{
	Node::setScale(fScale);
	SET_DIRTY_RECURSIVELY();
}

void Sprite::setScaleX(float fScaleX)
{
	Node::setScaleX(fScaleX);
	SET_DIRTY_RECURSIVELY();
}

void Sprite::setScaleY(float fScaleY)
{
	Node::setScaleY(fScaleY);
	SET_DIRTY_RECURSIVELY();
}

void Sprite::setRotation(float fRotation)
{
	Node::setRotation(fRotation);
	SET_DIRTY_RECURSIVELY();
}

void Sprite::setRotationX(float fRotationX)
{
	Node::setRotationX(fRotationX);
	SET_DIRTY_RECURSIVELY();
}

void Sprite::setRotationY(float fRotationY)
{
	Node::setRotationY(fRotationY);
	SET_DIRTY_RECURSIVELY();
}

void Sprite::setSkewX(float sx)
{
	Node::setSkewX(sx);
	SET_DIRTY_RECURSIVELY();
}

void Sprite::setSkewY(float sy)
{
	Node::setSkewY(sy);
	SET_DIRTY_RECURSIVELY();
}




void Sprite::setVertexZ(float fVertexZ)
{
	Node::setVertexZ(fVertexZ);
	SET_DIRTY_RECURSIVELY();
}

void Sprite::setAnchorPoint(const Point& anchor)
{
	Node::setAnchorPoint(anchor);
	SET_DIRTY_RECURSIVELY();
}

void Sprite::setVisible(bool bVisible)
{
	Node::setVisible(bVisible);
	SET_DIRTY_RECURSIVELY();
}







NS_CC_END
