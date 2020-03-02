#pragma once

#include "ccMacros.h"

NS_CC_BEGIN



class GLBufferedNode
{
#define BUFFER_SLOTS 4

public:
    GLBufferedNode(void);

    /**
     * Load the given data into this CCNode's GL Buffer. Needed for WebGL, as it does not support client-side arrays.
     */
    void setGLBufferData(void* buf, GLuint bufSize, int slot);
    void setGLIndexData(void* buf, GLuint bufSize, int slot);


public:
    // We allocate 4 buffer objs per node, and index into them as slots.
    GLuint _bufferObject[BUFFER_SLOTS];
    GLuint _bufferSize[BUFFER_SLOTS];

    GLuint _indexBufferObject[BUFFER_SLOTS];
    GLuint _indexBufferSize[BUFFER_SLOTS];
};




NS_CC_END