#include "GLBufferedNode.h"


NS_CC_BEGIN

GLBufferedNode::GLBufferedNode(void)
{
	for (int i = 0; i < BUFFER_SLOTS; i++)
	{
		_bufferObject[i] = 0;
		_bufferSize[i] = 0;
		_indexBufferObject[i] = 0;
		_indexBufferSize[i] = 0;
	}
}



void GLBufferedNode::setGLBufferData(void* buf, GLuint bufSize, int slot)
{
	// WebGL doesn't support client-side arrays, so generate a buffer and load the data first.
	if (_bufferSize[slot] < bufSize)
	{
		if (_bufferObject[slot])
		{
			glDeleteBuffers(1, &(_bufferObject[slot]));
		}
		glGenBuffers(1, &(_bufferObject[slot]));
		_bufferSize[slot] = bufSize;

		glBindBuffer(GL_ARRAY_BUFFER, _bufferObject[slot]);
		glBufferData(GL_ARRAY_BUFFER, bufSize, buf, GL_DYNAMIC_DRAW);
	}
	else
	{
		glBindBuffer(GL_ARRAY_BUFFER, _bufferObject[slot]);
		glBufferSubData(GL_ARRAY_BUFFER, 0, bufSize, buf);
	}
}

void GLBufferedNode::setGLIndexData(void* buf, GLuint bufSize, int slot)
{
	// WebGL doesn't support client-side arrays, so generate a buffer and load the data first.
	if (_indexBufferSize[slot] < bufSize)
	{
		if (_indexBufferObject[slot])
		{
			glDeleteBuffers(1, &(_indexBufferObject[slot]));
		}
		glGenBuffers(1, &(_indexBufferObject[slot]));
		_indexBufferSize[slot] = bufSize;

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _indexBufferObject[slot]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, bufSize, buf, GL_DYNAMIC_DRAW);
	}
	else
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _indexBufferObject[slot]);
		glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, bufSize, buf);
	}
}






NS_CC_END