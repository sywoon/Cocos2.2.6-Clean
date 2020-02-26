#include "GLStateCache.h"
#include "ccConfig.h"

NS_CC_BEGIN

static GLenum    s_eBlendingSource = -1;
static GLenum    s_eBlendingDest = -1;

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
#endif // CC_ENABLE_GL_STATE_CACHE
}




NS_CC_END