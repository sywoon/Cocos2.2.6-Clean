#pragma once

#include "ccMacros.h"
#include "cocoa/Geometry.h"
#include "GL/glew.h"



NS_CC_BEGIN


////////////////////////////////
// c3b 
//

typedef struct _ccColor3B
{
	GLubyte r;
	GLubyte g;
	GLubyte b;
} ccColor3B;

static inline ccColor3B
ccc3(const GLubyte r, const GLubyte g, const GLubyte b)
{
	ccColor3B c = { r, g, b };
	return c;
}

static inline bool ccc3BEqual(const ccColor3B& col1, const ccColor3B& col2)
{
	return col1.r == col2.r && col1.g == col2.g && col1.b == col2.b;
}

//ccColor3B predefined colors
static const ccColor3B ccWHITE = { 255,255,255 };
static const ccColor3B ccYELLOW = { 255,255,0 };
static const ccColor3B ccBLUE = { 0,0,255 };
static const ccColor3B ccGREEN = { 0,255,0 };
static const ccColor3B ccRED = { 255,0,0 };
// Magenta  洋红色
static const ccColor3B ccMAGENTA = { 255,0,255 };
static const ccColor3B ccBLACK = { 0,0,0 };
// Orange  橙色
static const ccColor3B ccORANGE = { 255,127,0 };
// Gray
static const ccColor3B ccGRAY = { 166,166,166 };


////////////////////////////////
// c4b 
//

typedef struct _ccColor4B
{
	GLubyte r;
	GLubyte g;
	GLubyte b;
	GLubyte a;
} ccColor4B;

static inline ccColor4B
ccc4(const GLubyte r, const GLubyte g, const GLubyte b, const GLubyte o)
{
	ccColor4B c = { r, g, b, o };
	return c;
}



////////////////////////////////
// c4f 
//

typedef struct _ccColor4F {
	GLfloat r;
	GLfloat g;
	GLfloat b;
	GLfloat a;
} ccColor4F;

static inline _ccColor4F
ccc4f(const GLfloat r, const GLfloat g, const GLfloat b, const GLfloat o)
{
	ccColor4F c = { r, g, b, o };
	return c;
}

static inline bool ccc4FEqual(ccColor4F a, ccColor4F b)
{
	return a.r == b.r && a.g == b.g && a.b == b.b && a.a == b.a;
}

static inline ccColor4F ccc4FFromccc3B(ccColor3B c)
{
	ccColor4F c4 = { c.r / 255.f, c.g / 255.f, c.b / 255.f, 1.f };
	return c4;
}

static inline ccColor4F ccc4FFromccc4B(ccColor4B c)
{
	ccColor4F c4 = { c.r / 255.f, c.g / 255.f, c.b / 255.f, c.a / 255.f };
	return c4;
}

static inline ccColor4B ccc4BFromccc4F(ccColor4F c)
{
	ccColor4B ret = { (GLubyte)(c.r * 255), (GLubyte)(c.g * 255), (GLubyte)(c.b * 255), (GLubyte)(c.a * 255) };
	return ret;
}


////////////////////////////////
// vec2
//

typedef struct _ccVertex2F
{
	GLfloat x;
	GLfloat y;
} ccVertex2F;

static inline ccVertex2F vertex2(const float x, const float y)
{
	ccVertex2F c = { x, y };
	return c;
}


////////////////////////////////
// vec3
//
typedef struct _ccVertex3F
{
	GLfloat x;
	GLfloat y;
	GLfloat z;
} ccVertex3F;

static inline ccVertex3F vertex3(const float x, const float y, const float z)
{
	ccVertex3F c = { x, y, z };
	return c;
}


////////////////////////////////
// tex2
//
typedef struct _ccTex2F {
	GLfloat u;
	GLfloat v;
} ccTex2F;

static inline ccTex2F tex2(const float u, const float v)
{
	ccTex2F t = { u , v };
	return t;
}


////////////////////////////////
// 各种opengl顶点结构 
//

typedef struct _ccPointSprite
{
	ccVertex2F    pos;        // 8 bytes
	ccColor4B     color;      // 4 bytes
	GLfloat       size;       // 4 bytes
} ccPointSprite;


// Quadrilateral 四边形
//!    A 2D Quad. 4 * 2 floats
typedef struct _ccQuad2 {
	ccVertex2F        tl;
	ccVertex2F        tr;
	ccVertex2F        bl;
	ccVertex2F        br;
} ccQuad2;

//!    A 3D Quad. 4 * 3 floats
typedef struct _ccQuad3 {
	ccVertex3F        bl;
	ccVertex3F        br;
	ccVertex3F        tl;
	ccVertex3F        tr;
} ccQuad3;

// texture coordinates for a quad
typedef struct _ccT2F_Quad
{
	//! bottom left
	ccTex2F    bl;
	//! bottom right
	ccTex2F    br;
	//! top left
	ccTex2F    tl;
	//! top right
	ccTex2F    tr;
} ccT2F_Quad;

//! a Point with a vertex point, a tex coord point and a color 4B
typedef struct _ccV2F_C4B_T2F
{
	//! vertices (2F)
	ccVertex2F        vertices;
	//! colors (4B)
	ccColor4B         colors;
	//! tex coords (2F)
	ccTex2F           texCoords;
} ccV2F_C4B_T2F;

//! a Point with a vertex point, a tex coord point and a color 4F
typedef struct _ccV2F_C4F_T2F
{
	//! vertices (2F)
	ccVertex2F        vertices;
	//! colors (4F)
	ccColor4F         colors;
	//! tex coords (2F)
	ccTex2F           texCoords;
} ccV2F_C4F_T2F;

typedef struct _ccV3F_C4B_T2F
{
	//! vertices (3F)
	ccVertex3F        vertices;            // 12 bytes
	//! colors (4B)
	ccColor4B         colors;              // 4 bytes
	// tex coords (2F)
	ccTex2F           texCoords;           // 8 bytes
} ccV3F_C4B_T2F;

//! A Triangle of ccV2F_C4B_T2F
typedef struct _ccV2F_C4B_T2F_Triangle
{
	//! Point A
	ccV2F_C4B_T2F a;
	//! Point B
	ccV2F_C4B_T2F b;
	//! Point B
	ccV2F_C4B_T2F c;
} ccV2F_C4B_T2F_Triangle;

//! A Quad of ccV2F_C4B_T2F
typedef struct _ccV2F_C4B_T2F_Quad
{
	//! bottom left
	ccV2F_C4B_T2F    bl;
	//! bottom right
	ccV2F_C4B_T2F    br;
	//! top left
	ccV2F_C4B_T2F    tl;
	//! top right
	ccV2F_C4B_T2F    tr;
} ccV2F_C4B_T2F_Quad;

//! 4 ccVertex3FTex2FColor4B
typedef struct _ccV3F_C4B_T2F_Quad
{
	//! top left
	ccV3F_C4B_T2F    tl;
	//! bottom left
	ccV3F_C4B_T2F    bl;
	//! top right
	ccV3F_C4B_T2F    tr;
	//! bottom right
	ccV3F_C4B_T2F    br;
} ccV3F_C4B_T2F_Quad;

//! 4 ccVertex2FTex2FColor4F Quad
typedef struct _ccV2F_C4F_T2F_Quad
{
	//! bottom left
	ccV2F_C4F_T2F    bl;
	//! bottom right
	ccV2F_C4F_T2F    br;
	//! top left
	ccV2F_C4F_T2F    tl;
	//! top right
	ccV2F_C4F_T2F    tr;
} ccV2F_C4F_T2F_Quad;

// struct that holds the size in pixels, texture coordinates and delays for animated CCParticleSystemQuad
typedef struct
{
	ccT2F_Quad texCoords;
	float delay;
	Size size;
} ccAnimationFrameData;


////////////////////////////////
// blend
//

//! Blend Function used for textures
typedef struct _ccBlendFunc
{
	//! source blend function
	GLenum src;
	//! destination blend function
	GLenum dst;
} ccBlendFunc;

static const ccBlendFunc kCCBlendFuncDisable = { GL_ONE, GL_ZERO };



////////////////////////////////
// alignment
//
typedef enum
{
	kCCVerticalTextAlignmentTop,
	kCCVerticalTextAlignmentCenter,
	kCCVerticalTextAlignmentBottom,
} CCVerticalTextAlignment;

typedef enum
{
	kCCTextAlignmentLeft,
	kCCTextAlignmentCenter,
	kCCTextAlignmentRight,
} CCTextAlignment;


////////////////////////////////
// font
//
typedef struct _ccFontShadow
{
public:
	// shadow is not enabled by default
	_ccFontShadow() : m_shadowEnabled(false)
					, m_shadowBlur(false)
					, m_shadowOpacity(1.0f)
					{}

	bool   m_shadowEnabled;
	// shadow x and y offset
	Size m_shadowOffset;
	// shadow blurrines
	float  m_shadowBlur;
	// shadow opacity
	float  m_shadowOpacity;

} ccFontShadow;

typedef struct _ccFontStroke
{
public:
	_ccFontStroke() : m_strokeEnabled(false)
					, m_strokeSize(1.0f)
					{}

	bool        m_strokeEnabled;
	ccColor3B   m_strokeColor;
	float       m_strokeSize;

} ccFontStroke;

typedef struct _ccFontDefinition
{
public:
	_ccFontDefinition() : m_alignment(kCCTextAlignmentCenter),
		m_vertAlignment(kCCVerticalTextAlignmentTop),
		m_fontFillColor(ccWHITE)
	{
		m_dimensions = SizeMake(0, 0);
	}

	std::string             m_fontName;
	int                     m_fontSize;
	// horizontal alignment
	CCTextAlignment         m_alignment;
	// vertical alignment
	CCVerticalTextAlignment m_vertAlignment;
	// renering box
	Size                    m_dimensions;
	// font color
	ccColor3B               m_fontFillColor;
	// font shadow
	ccFontShadow            m_shadow;
	// font stroke
	ccFontStroke            m_stroke;
} ccFontDefinition;




NS_CC_END
