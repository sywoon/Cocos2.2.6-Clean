#pragma once
#include "GL/glew.h"



NS_CC_BEGIN 


/** vertex attrib flags */
enum {
	kCCVertexAttribFlag_None = 0,

	kCCVertexAttribFlag_Position = 1 << 0,
	kCCVertexAttribFlag_Color = 1 << 1,
	kCCVertexAttribFlag_TexCoords = 1 << 2,

	kCCVertexAttribFlag_PosColorTex = (kCCVertexAttribFlag_Position | kCCVertexAttribFlag_Color | kCCVertexAttribFlag_TexCoords),
};




void CC_DLL ccGLBlendFunc(GLenum sfactor, GLenum dfactor);

void CC_DLL ccGLUseProgram(GLuint program);
void CC_DLL ccGLDeleteProgram(GLuint program);

void CC_DLL ccGLEnableVertexAttribs(unsigned int flags);



void CC_DLL ccGLBindVAO(GLuint vaoId);




NS_CC_END
