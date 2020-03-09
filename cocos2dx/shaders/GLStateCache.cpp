#include "GLStateCache.h"
#include "ccConfig.h"
#include "GLProgram.h"


NS_CC_BEGIN


static bool      s_bVertexAttribPosition = false;
static bool      s_bVertexAttribColor = false;
static bool      s_bVertexAttribTexCoords = false;


#if CC_ENABLE_GL_STATE_CACHE

static GLenum    s_eBlendingSource = -1;
static GLenum    s_eBlendingDest = -1;
static GLuint    s_uCurrentShaderProgram = -1;

#if CC_TEXTURE_ATLAS_USE_VAO
static GLuint    s_uVAO = 0;
#endif

#define kCCMaxActiveTexture 16
static GLuint    s_uCurrentBoundTexture[kCCMaxActiveTexture] = { (GLuint)-1,(GLuint)-1,(GLuint)-1,(GLuint)-1, (GLuint)-1,(GLuint)-1,(GLuint)-1,(GLuint)-1, (GLuint)-1,(GLuint)-1,(GLuint)-1,(GLuint)-1, (GLuint)-1,(GLuint)-1,(GLuint)-1,(GLuint)-1, };

#endif



static void SetBlending(GLenum sfactor, GLenum dfactor)
{
	if (sfactor == GL_ONE && dfactor == GL_ZERO)
	{
		glDisable(GL_BLEND);
	}
	else
	{
		glEnable(GL_BLEND);
		glBlendFunc(sfactor, dfactor);
	}
}

void ccGLBlendFunc(GLenum sfactor, GLenum dfactor)
{
#if CC_ENABLE_GL_STATE_CACHE
	if (sfactor != s_eBlendingSource || dfactor != s_eBlendingDest)
	{
		s_eBlendingSource = sfactor;
		s_eBlendingDest = dfactor;
		SetBlending(sfactor, dfactor);
	}
#else
	SetBlending(sfactor, dfactor);
#endif
}

void ccGLUseProgram(GLuint program)
{
#if CC_ENABLE_GL_STATE_CACHE
	if (program != s_uCurrentShaderProgram) {
		s_uCurrentShaderProgram = program;
		glUseProgram(program);
	}
#else
	glUseProgram(program);
#endif
}

void ccGLDeleteProgram(GLuint program)
{
#if CC_ENABLE_GL_STATE_CACHE
	if (program == s_uCurrentShaderProgram)
	{
		s_uCurrentShaderProgram = -1;
	}
#endif

	glDeleteProgram(program);
}



void ccGLEnableVertexAttribs(unsigned int flags)
{
	ccGLBindVAO(0);

	/* Position */
	bool enablePosition = flags & kCCVertexAttribFlag_Position;
	if (enablePosition != s_bVertexAttribPosition) {
		if (enablePosition)
			glEnableVertexAttribArray(kCCVertexAttrib_Position);
		else
			glDisableVertexAttribArray(kCCVertexAttrib_Position);

		s_bVertexAttribPosition = enablePosition;
	}

	/* Color */
	bool enableColor = (flags & kCCVertexAttribFlag_Color) != 0 ? true : false;

	if (enableColor != s_bVertexAttribColor) {
		if (enableColor)
			glEnableVertexAttribArray(kCCVertexAttrib_Color);
		else
			glDisableVertexAttribArray(kCCVertexAttrib_Color);

		s_bVertexAttribColor = enableColor;
	}

	/* Tex Coords */
	bool enableTexCoords = (flags & kCCVertexAttribFlag_TexCoords) != 0 ? true : false;

	if (enableTexCoords != s_bVertexAttribTexCoords) {
		if (enableTexCoords)
			glEnableVertexAttribArray(kCCVertexAttrib_TexCoords);
		else
			glDisableVertexAttribArray(kCCVertexAttrib_TexCoords);

		s_bVertexAttribTexCoords = enableTexCoords;
	}
}



void ccGLBindVAO(GLuint vaoId)
{
#if CC_TEXTURE_ATLAS_USE_VAO  

#if CC_ENABLE_GL_STATE_CACHE
	if (s_uVAO != vaoId)
	{
		s_uVAO = vaoId;
		glBindVertexArray(vaoId);
	}
#else
	glBindVertexArray(vaoId);
#endif

#endif
}


void ccGLBindTexture2D(GLuint textureId)
{
	ccGLBindTexture2DN(0, textureId);
}

void ccGLBindTexture2DN(GLuint textureUnit, GLuint textureId)
{
#if CC_ENABLE_GL_STATE_CACHE
	CCAssert(textureUnit < kCCMaxActiveTexture, "textureUnit is too big");
	if (s_uCurrentBoundTexture[textureUnit] != textureId)
	{
		s_uCurrentBoundTexture[textureUnit] = textureId;
		glActiveTexture(GL_TEXTURE0 + textureUnit);
		glBindTexture(GL_TEXTURE_2D, textureId);
	}
#else
	glActiveTexture(GL_TEXTURE0 + textureUnit);
	glBindTexture(GL_TEXTURE_2D, textureId);
#endif
}

void ccGLDeleteTexture(GLuint textureId)
{
	ccGLDeleteTextureN(0, textureId);
}

void ccGLDeleteTextureN(GLuint textureUnit, GLuint textureId)
{
#if CC_ENABLE_GL_STATE_CACHE
	if (s_uCurrentBoundTexture[textureUnit] == textureId)
	{
		s_uCurrentBoundTexture[textureUnit] = -1;
	}
#endif

	glDeleteTextures(1, &textureId);
}




NS_CC_END