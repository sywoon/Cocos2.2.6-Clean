#include "TextureAtlas.h"
#include "Texture2D.h"
#include "TextureCache.h"
#include "shaders/GLStateCache.h"
#include "shaders/GLProgram.h"


NS_CC_BEGIN


TextureAtlas* TextureAtlas::create(const char* file, unsigned int capacity)
{
	TextureAtlas* pTextureAtlas = new TextureAtlas();
	if (pTextureAtlas && pTextureAtlas->initWithFile(file, capacity))
	{
		pTextureAtlas->autorelease();
		return pTextureAtlas;
	}
	CC_SAFE_DELETE(pTextureAtlas);
	return NULL;
}

TextureAtlas* TextureAtlas::createWithTexture(Texture2D* texture, unsigned int capacity)
{
	TextureAtlas* pTextureAtlas = new TextureAtlas();
	if (pTextureAtlas && pTextureAtlas->initWithTexture(texture, capacity))
	{
		pTextureAtlas->autorelease();
		return pTextureAtlas;
	}
	CC_SAFE_DELETE(pTextureAtlas);
	return NULL;
}


TextureAtlas::TextureAtlas()
	: m_uTotalQuads(0)
	, m_uCapacity(0)

	, m_pTexture(NULL)
	, m_pQuads(NULL)

	, m_pIndices(NULL)
	, m_bDirty(false)
#if CC_TEXTURE_ATLAS_USE_VAO
	, m_uVAOname(0)
#endif
{
	memset(m_pBuffersVBO, 0, sizeof(m_pBuffersVBO));
}

TextureAtlas::~TextureAtlas()
{
	CCLOGINFO("cocos2d: CCTextureAtlas deallocing %p.", this);

	CC_SAFE_FREE(m_pQuads);
	CC_SAFE_FREE(m_pIndices);

	glDeleteBuffers(2, m_pBuffersVBO);

#if CC_TEXTURE_ATLAS_USE_VAO
	glDeleteVertexArrays(1, &m_uVAOname);
	ccGLBindVAO(0);
#endif
	CC_SAFE_RELEASE(m_pTexture);
}


unsigned int TextureAtlas::getTotalQuads()
{
	return m_uTotalQuads;
}

unsigned int TextureAtlas::getCapacity()
{
	return m_uCapacity;
}

Texture2D* TextureAtlas::getTexture()
{
	return m_pTexture;
}

void TextureAtlas::setTexture(Texture2D* var)
{
	CC_SAFE_RETAIN(var);
	CC_SAFE_RELEASE(m_pTexture);
	m_pTexture = var;
}

ccV3F_C4B_T2F_Quad* TextureAtlas::getQuads()
{
	//if someone accesses the quads directly, presume that changes will be made
	m_bDirty = true;
	return m_pQuads;
}

void TextureAtlas::setQuads(ccV3F_C4B_T2F_Quad* var)
{
	m_pQuads = var;
}

const char* TextureAtlas::description()
{
	return String::createWithFormat("<CCTextureAtlas | totalQuads = %u>", m_uTotalQuads)->getCString();
}


bool TextureAtlas::initWithFile(const char* file, unsigned int capacity)
{
	// retained in property
	Texture2D* texture = TextureCache::sharedTextureCache()->addImage(file);

	if (texture)
	{
		return initWithTexture(texture, capacity);
	}
	else
	{
		CCLOG("cocos2d: Could not open file: %s", file);
		return false;
	}
}


bool TextureAtlas::initWithTexture(Texture2D* texture, unsigned int capacity)
{
	m_uCapacity = capacity;
	m_uTotalQuads = 0;

	// retained in property
	this->m_pTexture = texture;
	CC_SAFE_RETAIN(m_pTexture);

	// Re-initialization is not allowed
	CCAssert(m_pQuads == NULL && m_pIndices == NULL, "");

	m_pQuads = (ccV3F_C4B_T2F_Quad*)malloc(m_uCapacity * sizeof(ccV3F_C4B_T2F_Quad));
	m_pIndices = (GLushort*)malloc(m_uCapacity * 6 * sizeof(GLushort));

	if (!(m_pQuads && m_pIndices) && m_uCapacity > 0)
	{
		//CCLOG("cocos2d: CCTextureAtlas: not enough memory");
		CC_SAFE_FREE(m_pQuads);
		CC_SAFE_FREE(m_pIndices);

		// release texture, should set it to null, because the destruction will
		// release it too. see cocos2d-x issue #484
		CC_SAFE_RELEASE_NULL(m_pTexture);
		return false;
	}

	memset(m_pQuads, 0, m_uCapacity * sizeof(ccV3F_C4B_T2F_Quad));
	memset(m_pIndices, 0, m_uCapacity * 6 * sizeof(GLushort));

	this->setupIndices();

#if CC_TEXTURE_ATLAS_USE_VAO
	setupVBOandVAO();
#else    
	setupVBO();
#endif

	m_bDirty = true;

	return true;
}



void TextureAtlas::setupIndices()
{
	if (m_uCapacity == 0)
		return;

	for (unsigned int i = 0; i < m_uCapacity; i++)
	{
#if CC_TEXTURE_ATLAS_USE_TRIANGLE_STRIP
		m_pIndices[i * 6 + 0] = i * 4 + 0;
		m_pIndices[i * 6 + 1] = i * 4 + 0;
		m_pIndices[i * 6 + 2] = i * 4 + 2;
		m_pIndices[i * 6 + 3] = i * 4 + 1;
		m_pIndices[i * 6 + 4] = i * 4 + 3;
		m_pIndices[i * 6 + 5] = i * 4 + 3;
#else
		m_pIndices[i * 6 + 0] = i * 4 + 0;
		m_pIndices[i * 6 + 1] = i * 4 + 1;
		m_pIndices[i * 6 + 2] = i * 4 + 2;

		// inverted index
		m_pIndices[i * 6 + 3] = i * 4 + 3;
		m_pIndices[i * 6 + 4] = i * 4 + 2;
		m_pIndices[i * 6 + 5] = i * 4 + 1;
#endif    
	}
}


#if CC_TEXTURE_ATLAS_USE_VAO
void TextureAtlas::setupVBOandVAO()
{
	glGenVertexArrays(1, &m_uVAOname);
	ccGLBindVAO(m_uVAOname);

#define kQuadSize sizeof(m_pQuads[0].bl)

	glGenBuffers(2, &m_pBuffersVBO[0]);

	glBindBuffer(GL_ARRAY_BUFFER, m_pBuffersVBO[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(m_pQuads[0]) * m_uCapacity, m_pQuads, GL_DYNAMIC_DRAW);

	// vertices
	glEnableVertexAttribArray(kCCVertexAttrib_Position);
	glVertexAttribPointer(kCCVertexAttrib_Position, 3, GL_FLOAT, GL_FALSE, kQuadSize, (GLvoid*)offsetof(ccV3F_C4B_T2F, vertices));

	// colors
	glEnableVertexAttribArray(kCCVertexAttrib_Color);
	glVertexAttribPointer(kCCVertexAttrib_Color, 4, GL_UNSIGNED_BYTE, GL_TRUE, kQuadSize, (GLvoid*)offsetof(ccV3F_C4B_T2F, colors));

	// tex coords
	glEnableVertexAttribArray(kCCVertexAttrib_TexCoords);
	glVertexAttribPointer(kCCVertexAttrib_TexCoords, 2, GL_FLOAT, GL_FALSE, kQuadSize, (GLvoid*)offsetof(ccV3F_C4B_T2F, texCoords));

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_pBuffersVBO[1]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(m_pIndices[0]) * m_uCapacity * 6, m_pIndices, GL_STATIC_DRAW);

	// Must unbind the VAO before changing the element buffer.
	ccGLBindVAO(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	CHECK_GL_ERROR_DEBUG();
}
#else // CC_TEXTURE_ATLAS_USE_VAO
void TextureAtlas::setupVBO()
{
	glGenBuffers(2, &m_pBuffersVBO[0]);

	mapBuffers();
}
#endif

void TextureAtlas::mapBuffers()
{
	// Avoid changing the element buffer for whatever VAO might be bound.
	ccGLBindVAO(0);

	glBindBuffer(GL_ARRAY_BUFFER, m_pBuffersVBO[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(m_pQuads[0]) * m_uCapacity, m_pQuads, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_pBuffersVBO[1]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(m_pIndices[0]) * m_uCapacity * 6, m_pIndices, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	CHECK_GL_ERROR_DEBUG();
}


//////////////////////////////////////////////////
// TextureAtlas - Update, Insert, Move & Remove
//
void TextureAtlas::updateQuad(ccV3F_C4B_T2F_Quad* quad, unsigned int index)
{
	CCAssert(index >= 0 && index < m_uCapacity, "updateQuadWithTexture: Invalid index");

	m_uTotalQuads = MAX(index + 1, m_uTotalQuads);
	m_pQuads[index] = *quad;

	m_bDirty = true;
}


void TextureAtlas::insertQuad(ccV3F_C4B_T2F_Quad* quad, unsigned int index)
{
	CCAssert(index < m_uCapacity, "insertQuadWithTexture: Invalid index");

	m_uTotalQuads++;
	CCAssert(m_uTotalQuads <= m_uCapacity, "invalid totalQuads");

	// index can be > totalQuads
	unsigned int remaining = (m_uTotalQuads - 1) - index;

	// last object doesn't need to be moved  ?为什么最后一个元素不要了
	if (remaining > 0)
	{
		// texture coordinates
		memmove(&m_pQuads[index + 1], &m_pQuads[index], sizeof(m_pQuads[0]) * remaining);
	}

	m_pQuads[index] = *quad;


	m_bDirty = true;

}

void TextureAtlas::insertQuads(ccV3F_C4B_T2F_Quad* quads, unsigned int index, unsigned int amount)
{
	CCAssert(index + amount <= m_uCapacity, "insertQuadWithTexture: Invalid index + amount");

	m_uTotalQuads += amount;

	CCAssert(m_uTotalQuads <= m_uCapacity, "invalid totalQuads");

	// index can be > totalQuads
	int remaining = (m_uTotalQuads - 1) - index - amount;

	// last object doesn't need to be moved
	if (remaining > 0)
	{
		// tex coordinates
		memmove(&m_pQuads[index + amount], &m_pQuads[index], sizeof(m_pQuads[0]) * remaining);
	}

	unsigned int max = index + amount;
	unsigned int j = 0;
	for (unsigned int i = index; i < max; i++)
	{
		m_pQuads[index] = quads[j];
		index++;
		j++;
	}

	m_bDirty = true;
}

void TextureAtlas::insertQuadFromIndex(unsigned int oldIndex, unsigned int newIndex)
{
	CCAssert(newIndex >= 0 && newIndex < m_uTotalQuads, "insertQuadFromIndex:atIndex: Invalid index");
	CCAssert(oldIndex >= 0 && oldIndex < m_uTotalQuads, "insertQuadFromIndex:atIndex: Invalid index");

	if (oldIndex == newIndex)
		return;

	// because it is ambiguous in iphone, so we implement abs ourselves
	// unsigned int howMany = abs( oldIndex - newIndex);
	unsigned int howMany = (oldIndex - newIndex) > 0 ? (oldIndex - newIndex) : (newIndex - oldIndex);
	unsigned int dst = oldIndex;
	unsigned int src = oldIndex + 1;
	if (oldIndex > newIndex)
	{
		dst = newIndex + 1;
		src = newIndex;
	}

	// texture coordinates
	ccV3F_C4B_T2F_Quad quadsBackup = m_pQuads[oldIndex];
	memmove(&m_pQuads[dst], &m_pQuads[src], sizeof(m_pQuads[0]) * howMany);
	m_pQuads[newIndex] = quadsBackup;

	m_bDirty = true;
}


void TextureAtlas::removeQuadAtIndex(unsigned int index)
{
	CCAssert(index < m_uTotalQuads, "removeQuadAtIndex: Invalid index");

	int remaining = (m_uTotalQuads - 1) - index;

	// last object doesn't need to be moved
	if (remaining > 0)
	{
		// texture coordinates
		memmove(&m_pQuads[index], &m_pQuads[index + 1], sizeof(m_pQuads[0]) * remaining);
	}

	m_uTotalQuads--;
	m_bDirty = true;
}

void TextureAtlas::removeQuadsAtIndex(unsigned int index, unsigned int amount)
{
	CCAssert(index + amount <= m_uTotalQuads, "removeQuadAtIndex: index + amount out of bounds");

	int remaining = (m_uTotalQuads) - (index + amount);
	m_uTotalQuads -= amount;

	if (remaining > 0)
	{
		memmove(&m_pQuads[index], &m_pQuads[index + amount], sizeof(m_pQuads[0]) * remaining);
	}

	m_bDirty = true;
}

void TextureAtlas::removeAllQuads()
{
	m_uTotalQuads = 0;
}


void TextureAtlas::moveQuadsFromIndex(unsigned int oldIndex, unsigned int amount, unsigned int newIndex)
{
	CCAssert(newIndex + amount <= m_uTotalQuads, "insertQuadFromIndex:atIndex: Invalid index");
	CCAssert(oldIndex < m_uTotalQuads, "insertQuadFromIndex:atIndex: Invalid index");

	if (oldIndex == newIndex)
		return;

	//create buffer
	size_t quadSize = sizeof(ccV3F_C4B_T2F_Quad);
	ccV3F_C4B_T2F_Quad* tempQuads = (ccV3F_C4B_T2F_Quad*)malloc(quadSize * amount);
	memcpy(tempQuads, &m_pQuads[oldIndex], quadSize * amount);

	if (newIndex < oldIndex)
	{
		// move quads from newIndex to newIndex + amount to make room for buffer
		memmove(&m_pQuads[newIndex], &m_pQuads[newIndex + amount], (oldIndex - newIndex) * quadSize);
	}
	else
	{
		// move quads above back
		memmove(&m_pQuads[oldIndex], &m_pQuads[oldIndex + amount], (newIndex - oldIndex) * quadSize);
	}
	memcpy(&m_pQuads[newIndex], tempQuads, amount * quadSize);

	free(tempQuads);

	m_bDirty = true;
}

void TextureAtlas::moveQuadsFromIndex(unsigned int index, unsigned int newIndex)
{
	CCAssert(newIndex + (m_uTotalQuads - index) <= m_uCapacity, "moveQuadsFromIndex move is out of bounds");

	memmove(m_pQuads + newIndex, m_pQuads + index, (m_uTotalQuads - index) * sizeof(m_pQuads[0]));
}

void TextureAtlas::fillWithEmptyQuadsFromIndex(unsigned int index, unsigned int amount)
{
	ccV3F_C4B_T2F_Quad quad;
	memset(&quad, 0, sizeof(quad));

	unsigned int to = index + amount;
	for (unsigned int i = index; i < to; i++)
	{
		m_pQuads[i] = quad;
	}
}



/////////////////////////////////
// TextureAtlas - Resize
//
bool TextureAtlas::resizeCapacity(unsigned int newCapacity)
{
	if (newCapacity == m_uCapacity)
		return true;

	unsigned int uOldCapactiy = m_uCapacity;
	// update capacity and totalQuads
	m_uTotalQuads = MIN(m_uTotalQuads, newCapacity);
	m_uCapacity = newCapacity;

	ccV3F_C4B_T2F_Quad* tmpQuads = NULL;
	GLushort* tmpIndices = NULL;

	// when calling initWithTexture(fileName, 0) on bada device, calloc(0, 1) will fail and return NULL,
	// so here must judge whether m_pQuads and m_pIndices is NULL.
	if (m_pQuads == NULL)
	{
		tmpQuads = (ccV3F_C4B_T2F_Quad*)malloc(m_uCapacity * sizeof(m_pQuads[0]));
		if (tmpQuads != NULL)
		{
			memset(tmpQuads, 0, m_uCapacity * sizeof(m_pQuads[0]));
		}
	}
	else
	{
		tmpQuads = (ccV3F_C4B_T2F_Quad*)realloc(m_pQuads, sizeof(m_pQuads[0]) * m_uCapacity);
		if (tmpQuads != NULL && m_uCapacity > uOldCapactiy)
		{
			memset(tmpQuads + uOldCapactiy, 0, (m_uCapacity - uOldCapactiy) * sizeof(m_pQuads[0]));
		}
	}

	if (m_pIndices == NULL)
	{
		tmpIndices = (GLushort*)malloc(m_uCapacity * 6 * sizeof(m_pIndices[0]));
		if (tmpIndices != NULL)
		{
			memset(tmpIndices, 0, m_uCapacity * 6 * sizeof(m_pIndices[0]));
		}

	}
	else
	{
		tmpIndices = (GLushort*)realloc(m_pIndices, sizeof(m_pIndices[0]) * m_uCapacity * 6);
		if (tmpIndices != NULL && m_uCapacity > uOldCapactiy)
		{
			memset(tmpIndices + uOldCapactiy, 0, (m_uCapacity - uOldCapactiy) * 6 * sizeof(m_pIndices[0]));
		}
	}

	if (!(tmpQuads && tmpIndices)) {
		CCLOG("cocos2d: CCTextureAtlas: not enough memory");
		CC_SAFE_FREE(tmpQuads);
		CC_SAFE_FREE(tmpIndices);
		CC_SAFE_FREE(m_pQuads);
		CC_SAFE_FREE(m_pIndices);
		m_uCapacity = m_uTotalQuads = 0;
		return false;
	}

	m_pQuads = tmpQuads;
	m_pIndices = tmpIndices;

	setupIndices();
	mapBuffers();

	m_bDirty = true;

	return true;
}

void TextureAtlas::increaseTotalQuadsWith(unsigned int amount)
{
	m_uTotalQuads += amount;
}


///////////////////////////////
// TextureAtlas - Drawing
//

void TextureAtlas::drawQuads()
{
	this->drawNumberOfQuads(m_uTotalQuads, 0);
}

void TextureAtlas::drawNumberOfQuads(unsigned int n)
{
	this->drawNumberOfQuads(n, 0);
}

void TextureAtlas::drawNumberOfQuads(unsigned int n, unsigned int start)
{
	if (0 == n)
	{
		return;
	}
	ccGLBindTexture2D(m_pTexture->getName());

#if CC_TEXTURE_ATLAS_USE_VAO

	//
	// Using VBO and VAO
	//

	// XXX: update is done in draw... perhaps it should be done in a timer
	if (m_bDirty)
	{
		glBindBuffer(GL_ARRAY_BUFFER, m_pBuffersVBO[0]);
		// option 1: subdata
		//glBufferSubData(GL_ARRAY_BUFFER, sizeof(m_pQuads[0])*start, sizeof(m_pQuads[0]) * n , &m_pQuads[start] );

		// option 2: data
		//		glBufferData(GL_ARRAY_BUFFER, sizeof(quads_[0]) * (n-start), &quads_[start], GL_DYNAMIC_DRAW);

		// option 3: orphaning + glMapBuffer
		glBufferData(GL_ARRAY_BUFFER, sizeof(m_pQuads[0]) * (n - start), NULL, GL_DYNAMIC_DRAW);
		void* buf = glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
		memcpy(buf, m_pQuads, sizeof(m_pQuads[0]) * (n - start));
		glUnmapBuffer(GL_ARRAY_BUFFER);

		glBindBuffer(GL_ARRAY_BUFFER, 0);

		m_bDirty = false;
	}

	ccGLBindVAO(m_uVAOname);

#if CC_REBIND_INDICES_BUFFER
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_pBuffersVBO[1]);
#endif

#if CC_TEXTURE_ATLAS_USE_TRIANGLE_STRIP
	glDrawElements(GL_TRIANGLE_STRIP, (GLsizei)n * 6, GL_UNSIGNED_SHORT, (GLvoid*)(start * 6 * sizeof(m_pIndices[0])));
#else
	glDrawElements(GL_TRIANGLES, (GLsizei)n * 6, GL_UNSIGNED_SHORT, (GLvoid*)(start * 6 * sizeof(m_pIndices[0])));
#endif // CC_TEXTURE_ATLAS_USE_TRIANGLE_STRIP

#if CC_REBIND_INDICES_BUFFER
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
#endif

	//    glBindVertexArray(0);

#else // ! CC_TEXTURE_ATLAS_USE_VAO

	//
	// Using VBO without VAO
	//

#define kQuadSize sizeof(m_pQuads[0].bl)
	glBindBuffer(GL_ARRAY_BUFFER, m_pBuffersVBO[0]);

	// XXX: update is done in draw... perhaps it should be done in a timer
	if (m_bDirty)
	{
		glBufferSubData(GL_ARRAY_BUFFER, sizeof(m_pQuads[0]) * start, sizeof(m_pQuads[0]) * n, &m_pQuads[start]);
		m_bDirty = false;
	}

	ccGLEnableVertexAttribs(kCCVertexAttribFlag_PosColorTex);

	// vertices
	glVertexAttribPointer(kCCVertexAttrib_Position, 3, GL_FLOAT, GL_FALSE, kQuadSize, (GLvoid*)offsetof(ccV3F_C4B_T2F, vertices));

	// colors
	glVertexAttribPointer(kCCVertexAttrib_Color, 4, GL_UNSIGNED_BYTE, GL_TRUE, kQuadSize, (GLvoid*)offsetof(ccV3F_C4B_T2F, colors));

	// tex coords
	glVertexAttribPointer(kCCVertexAttrib_TexCoords, 2, GL_FLOAT, GL_FALSE, kQuadSize, (GLvoid*)offsetof(ccV3F_C4B_T2F, texCoords));

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_pBuffersVBO[1]);

#if CC_TEXTURE_ATLAS_USE_TRIANGLE_STRIP
	glDrawElements(GL_TRIANGLE_STRIP, (GLsizei)n * 6, GL_UNSIGNED_SHORT, (GLvoid*)(start * 6 * sizeof(m_pIndices[0])));
#else
	glDrawElements(GL_TRIANGLES, (GLsizei)n * 6, GL_UNSIGNED_SHORT, (GLvoid*)(start * 6 * sizeof(m_pIndices[0])));
#endif // CC_TEXTURE_ATLAS_USE_TRIANGLE_STRIP

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

#endif // CC_TEXTURE_ATLAS_USE_VAO

	CC_INCREMENT_GL_DRAWS(1);
	CHECK_GL_ERROR_DEBUG();
}


void TextureAtlas::listenBackToForeground(Object* obj)
{
#if CC_TEXTURE_ATLAS_USE_VAO
	setupVBOandVAO();
#else    
	setupVBO();
#endif

	// set m_bDirty to true to force it rebinding buffer
	m_bDirty = true;
}

NS_CC_END