#pragma once

#include "ccMacros.h"
#include "cocoa/Object.h"


NS_CC_BEGIN


class Texture2D;
class CC_DLL TextureAtlas : public Object
{
public:
    TextureAtlas();
    virtual ~TextureAtlas();

    static TextureAtlas* create(const char* file, unsigned int capacity);
    static TextureAtlas* createWithTexture(Texture2D* texture, unsigned int capacity);


public:
    const char*     description();

    void            updateQuad(ccV3F_C4B_T2F_Quad* quad, unsigned int index);
    void            insertQuad(ccV3F_C4B_T2F_Quad* quad, unsigned int index);
    void            insertQuads(ccV3F_C4B_T2F_Quad* quads, unsigned int index, unsigned int amount);
    // Removes the quad that is located at a certain index and inserts it at a new index
    void            insertQuadFromIndex(unsigned int fromIndex, unsigned int newIndex);
    void            removeQuadAtIndex(unsigned int index);
    void            removeQuadsAtIndex(unsigned int index, unsigned int amount);
    void            removeAllQuads();
	void            moveQuadsFromIndex(unsigned int oldIndex, unsigned int amount, unsigned int newIndex);
	void            moveQuadsFromIndex(unsigned int index, unsigned int newIndex);
	void            fillWithEmptyQuadsFromIndex(unsigned int index, unsigned int amount);


    bool            resizeCapacity(unsigned int n);
    void            increaseTotalQuadsWith(unsigned int amount);

    void            drawNumberOfQuads(unsigned int n);
    void            drawNumberOfQuads(unsigned int n, unsigned int start);
    void            drawQuads();

	inline bool     isDirty(void) { return m_bDirty; }
	inline void     setDirty(bool bDirty) { m_bDirty = bDirty; }

    void            listenBackToForeground(Object* obj);


public:
    bool            initWithFile(const char* file, unsigned int capacity);
    bool            initWithTexture(Texture2D* texture, unsigned int capacity);


private:
	void            setupIndices();

#if CC_TEXTURE_ATLAS_USE_VAO
	void            setupVBOandVAO();
#else
	void            setupVBO();
#endif

    void            mapBuffers();



private:
    CC_PROPERTY_READONLY(unsigned int, m_uTotalQuads, TotalQuads)
    CC_PROPERTY_READONLY(unsigned int, m_uCapacity, Capacity)

    CC_PROPERTY(Texture2D*, m_pTexture, Texture)
    CC_PROPERTY(ccV3F_C4B_T2F_Quad*, m_pQuads, Quads)

    GLuint      m_pBuffersVBO[2]; //0: vertex  1: indices
    GLushort*   m_pIndices;

#if CC_TEXTURE_ATLAS_USE_VAO
	GLuint      m_uVAOname;
#endif

    bool        m_bDirty;
};



NS_CC_END
